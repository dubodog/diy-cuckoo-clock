"""Compile and run host tests; no hardware or Arduino libraries required."""
from pathlib import Path
import os
import subprocess
import tempfile
root = Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory(prefix="cuckoo-test-") as tmp:
    exe = str(Path(tmp) / "test")
    subprocess.run([os.environ.get("CXX", "c++"), "-std=c++17", "-I", str(root / "tests/host"), str(root / "tests/host/test.cpp"), "-o", exe], check=True)
    subprocess.run([exe], check=True, timeout=30)
