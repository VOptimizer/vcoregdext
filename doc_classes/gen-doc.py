import sys
sys.path.append("../third-party/godot-cpp")

from doc_source_generator import generate_doc_source
from pathlib import Path

path = Path("./")

generate_doc_source("./doc_data.gen.cpp", path.rglob('*.xml'))