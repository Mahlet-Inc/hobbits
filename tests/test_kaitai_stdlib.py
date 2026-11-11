"""Import every bundled parser with site packages disabled (-S)."""
import importlib.util
from pathlib import Path
import sys

root = Path(sys.argv[1])
parsers = sorted((root / "ksy_py").rglob("*.py"))
sys.path.insert(0, str(root / "scripts" / "dependencies"))
sys.path.extend(sorted({str(path.parent) for path in parsers}))

for index, path in enumerate(parsers):
    spec = importlib.util.spec_from_file_location("hobbits_kaitai_test_" + str(index), path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)

print("Imported", len(parsers), "Kaitai parsers using only standard Python and the bundled runtime")
