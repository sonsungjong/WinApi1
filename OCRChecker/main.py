import base64
import logging
import re
from pathlib import Path
from threading import Lock
from typing import List, Optional

from fastapi import Body, FastAPI
from pydantic import BaseModel
from openpyxl import Workbook, load_workbook


APP_NAME = "OCR Image Receiver"
RESULT_DIR = Path(r"C:\result").resolve()
LOG_XLSX_PATH = Path(__file__).resolve().parent / "활동데이터_견적서템플릿.xlsx"
_EXCEL_LOCK = Lock()


class ImagePayload(BaseModel):
    filename: str
    data: str  # base64 string; may also be a data URI


class CancelRequest(BaseModel):
    reason: Optional[str] = None


app = FastAPI(title=APP_NAME)


class ResponseItem(BaseModel):
    itemNo: int
    item: str
    name: str
    spec: str
    quantity: str
    unitPrice: str
    finalAmount: str
    manufacturer: str
    size: str


def ensure_result_dir() -> None:
    RESULT_DIR.mkdir(parents=True, exist_ok=True)


_WINDOWS_RESERVED = {
    "CON",
    "PRN",
    "AUX",
    "NUL",
    *(f"COM{i}" for i in range(1, 10)),
    *(f"LPT{i}" for i in range(1, 10)),
}


def sanitize_filename(original_name: str) -> str:
    # Keep only the base name and strip whitespace
    name_only = Path(original_name).name.strip()
    # Replace characters illegal on Windows
    name_only = re.sub(r'[<>:"/\\|?*\x00-\x1F]', "_", name_only)
    # Avoid reserved device names
    stem = Path(name_only).stem.upper()
    if stem in _WINDOWS_RESERVED:
        name_only = f"_{name_only}"
    # Ensure not empty
    return name_only or "unnamed.png"


def decode_base64_maybe_data_uri(data: str) -> bytes:
    raw = data.strip()
    # Support data URIs like: data:image/png;base64,AAAA
    if "," in raw and ";base64" in raw.split(",", 1)[0]:
        raw = raw.split(",", 1)[1]
    # Fix missing padding
    padding_len = (-len(raw)) % 4
    if padding_len:
        raw += "=" * padding_len
    return base64.b64decode(raw, validate=False)


def save_image_to_result_dir(filename: str, content: bytes) -> Path:
    safe_name = sanitize_filename(filename)
    # Default extension to .png if absent
    if not Path(safe_name).suffix:
        safe_name = f"{safe_name}.png"
    dest = RESULT_DIR / safe_name
    with open(dest, "wb") as f:
        f.write(content)
    return dest


def handle_images(payload: List[ImagePayload]) -> List[str]:
    ensure_result_dir()
    saved_names: List[str] = []
    for item in payload:
        try:
            binary = decode_base64_maybe_data_uri(item.data)
        except Exception as e:
            logging.exception("Failed to decode base64 for %s", item.filename)
            print(f"[RECV] decode error: {item.filename}")
            saved_names.append(sanitize_filename(item.filename))
            continue

        try:
            dest = save_image_to_result_dir(item.filename, binary)
            print(f"[SAVE] {item.filename} -> {dest}")
            saved_names.append(dest.name)
        except Exception as e:
            logging.exception("Failed to save file %s", item.filename)
            print(f"[SAVE-ERROR] {item.filename}: {e}")
            saved_names.append(sanitize_filename(item.filename))
    return saved_names


def build_response_items(saved_names: List[str]) -> List[ResponseItem]:
    items: List[ResponseItem] = []
    for idx, name in enumerate(saved_names, start=1):
        items.append(
            ResponseItem(
                itemNo=idx,
                item=name,
                name=name,
                spec=name,
                quantity=name,
                unitPrice=name,
                finalAmount=name,
                manufacturer=name,
                size=name,
            )
        )
    return items


def append_to_excel(items: List[ResponseItem]) -> None:
    try:
        _EXCEL_LOCK.acquire()
        if LOG_XLSX_PATH.exists():
            wb = load_workbook(LOG_XLSX_PATH)
            ws = wb.active
        else:
            wb = Workbook()
            ws = wb.active
            ws.append(["itemNo", "item", "name", "spec", "quantity", "unitPrice", "finalAmount", "manufacturer", "size"])
        for it in items:
            ws.append([it.itemNo, it.item, it.name, it.spec, it.quantity, it.unitPrice, it.finalAmount, it.manufacturer, it.size])
        wb.save(LOG_XLSX_PATH)
    except Exception:
        logging.exception("Failed to append responses to Excel: %s", LOG_XLSX_PATH)
    finally:
        try:
            _EXCEL_LOCK.release()
        except Exception:
            pass


@app.get("/health")
def health() -> dict:
    return {"status": "ok", "result_dir": str(RESULT_DIR)}


@app.post("/ocr", response_model=List[ResponseItem])
def upload_ocr(payload: List[ImagePayload] = Body(...)) -> List[ResponseItem]:
    try:
        print(f"[RECV] /ocr items={len(payload)} files={[p.filename for p in payload]}")
    except Exception:
        print("[RECV] /ocr (unable to print filenames)")
    saved_names = handle_images(payload)
    items = build_response_items(saved_names)
    append_to_excel(items)
    return items


# Also accept POST at root for convenience
@app.post("/", response_model=List[ResponseItem])
def upload_root(payload: List[ImagePayload] = Body(...)) -> List[ResponseItem]:
    try:
        print(f"[RECV] / items={len(payload)} files={[p.filename for p in payload]}")
    except Exception:
        print("[RECV] / (unable to print filenames)")
    saved_names = handle_images(payload)
    items = build_response_items(saved_names)
    append_to_excel(items)
    return items


@app.post("/cancel", response_model=str)
def cancel(req: CancelRequest) -> str:
    reason = (req.reason or "").strip()
    logging.info("Cancel requested%s", f": {reason}" if reason else "")
    print(f"[CANCEL] reason='{reason}'")
    return "CANCELLED"


if __name__ == "__main__":
    import uvicorn

    uvicorn.run(app, host="0.0.0.0", port=18333)


