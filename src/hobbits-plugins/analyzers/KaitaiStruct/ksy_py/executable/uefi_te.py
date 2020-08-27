# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
import collections
from enum import Enum


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

class UefiTe(KaitaiStruct):
    """This type of executables could be found inside the UEFI firmware. The UEFI 
    firmware is stored in SPI flash memory, which is a chip soldered on a 
    system’s motherboard. UEFI firmware is very modular: it usually contains 
    dozens, if not hundreds, of executables. To store all these separates files, 
    the firmware is laid out in volumes using the Firmware File System (FFS), a 
    file system specifically designed to store firmware images. The volumes 
    contain files that are identified by GUIDs and each of these files contain 
    one or more sections holding the data. One of these sections contains the 
    actual executable image. Most of the executable images follow the PE format. 
    However, some of them follow the TE format.
    
    The Terse Executable (TE) image format was created as a mechanism to reduce
    the overhead of the PE/COFF headers in PE32/PE32+ images, resulting in a 
    corresponding reduction of image sizes for executables running in the PI 
    (Platform Initialization) Architecture environment. Reducing image size 
    provides an opportunity for use of a smaller system flash part.
    
    So the TE format is basically a stripped version of PE.
    
    .. seealso::
       Source - https://uefi.org/sites/default/files/resources/PI_Spec_1_6.pdf
    """
    SEQ_FIELDS = ["te_hdr", "sections"]
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._debug = collections.defaultdict(dict)

    def _read(self):
        self._debug['te_hdr']['start'] = self._io.pos()
        self._raw_te_hdr = self._io.read_bytes(40)
        _io__raw_te_hdr = KaitaiStream(BytesIO(self._raw_te_hdr))
        self.te_hdr = self._root.TeHeader(_io__raw_te_hdr, self, self._root)
        self.te_hdr._read()
        self._debug['te_hdr']['end'] = self._io.pos()
        self._debug['sections']['start'] = self._io.pos()
        self.sections = [None] * (self.te_hdr.num_sections)
        for i in range(self.te_hdr.num_sections):
            if not 'arr' in self._debug['sections']:
                self._debug['sections']['arr'] = []
            self._debug['sections']['arr'].append({'start': self._io.pos()})
            _t_sections = self._root.Section(self._io, self, self._root)
            _t_sections._read()
            self.sections[i] = _t_sections
            self._debug['sections']['arr'][i]['end'] = self._io.pos()

        self._debug['sections']['end'] = self._io.pos()

    class TeHeader(KaitaiStruct):

        class MachineType(Enum):
            unknown = 0
            i386 = 332
            r4000 = 358
            wcemipsv2 = 361
            alpha = 388
            sh3 = 418
            sh3dsp = 419
            sh4 = 422
            sh5 = 424
            arm = 448
            thumb = 450
            armnt = 452
            am33 = 467
            powerpc = 496
            powerpcfp = 497
            ia64 = 512
            mips16 = 614
            mipsfpu = 870
            mipsfpu16 = 1126
            ebc = 3772
            riscv32 = 20530
            riscv64 = 20580
            riscv128 = 20776
            amd64 = 34404
            m32r = 36929
            arm64 = 43620

        class SubsystemEnum(Enum):
            unknown = 0
            native = 1
            windows_gui = 2
            windows_cui = 3
            posix_cui = 7
            windows_ce_gui = 9
            efi_application = 10
            efi_boot_service_driver = 11
            efi_runtime_driver = 12
            efi_rom = 13
            xbox = 14
            windows_boot_application = 16
        SEQ_FIELDS = ["magic", "machine", "num_sections", "subsystem", "stripped_size", "entry_point_addr", "base_of_code", "image_base", "data_dirs"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['magic']['start'] = self._io.pos()
            self.magic = self._io.read_bytes(2)
            self._debug['magic']['end'] = self._io.pos()
            if not self.magic == b"\x56\x5A":
                raise kaitaistruct.ValidationNotEqualError(b"\x56\x5A", self.magic, self._io, u"/types/te_header/seq/0")
            self._debug['machine']['start'] = self._io.pos()
            self.machine = KaitaiStream.resolve_enum(self._root.TeHeader.MachineType, self._io.read_u2le())
            self._debug['machine']['end'] = self._io.pos()
            self._debug['num_sections']['start'] = self._io.pos()
            self.num_sections = self._io.read_u1()
            self._debug['num_sections']['end'] = self._io.pos()
            self._debug['subsystem']['start'] = self._io.pos()
            self.subsystem = KaitaiStream.resolve_enum(self._root.TeHeader.SubsystemEnum, self._io.read_u1())
            self._debug['subsystem']['end'] = self._io.pos()
            self._debug['stripped_size']['start'] = self._io.pos()
            self.stripped_size = self._io.read_u2le()
            self._debug['stripped_size']['end'] = self._io.pos()
            self._debug['entry_point_addr']['start'] = self._io.pos()
            self.entry_point_addr = self._io.read_u4le()
            self._debug['entry_point_addr']['end'] = self._io.pos()
            self._debug['base_of_code']['start'] = self._io.pos()
            self.base_of_code = self._io.read_u4le()
            self._debug['base_of_code']['end'] = self._io.pos()
            self._debug['image_base']['start'] = self._io.pos()
            self.image_base = self._io.read_u8le()
            self._debug['image_base']['end'] = self._io.pos()
            self._debug['data_dirs']['start'] = self._io.pos()
            self.data_dirs = self._root.HeaderDataDirs(self._io, self, self._root)
            self.data_dirs._read()
            self._debug['data_dirs']['end'] = self._io.pos()


    class HeaderDataDirs(KaitaiStruct):
        SEQ_FIELDS = ["base_relocation_table", "debug"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['base_relocation_table']['start'] = self._io.pos()
            self.base_relocation_table = self._root.DataDir(self._io, self, self._root)
            self.base_relocation_table._read()
            self._debug['base_relocation_table']['end'] = self._io.pos()
            self._debug['debug']['start'] = self._io.pos()
            self.debug = self._root.DataDir(self._io, self, self._root)
            self.debug._read()
            self._debug['debug']['end'] = self._io.pos()


    class DataDir(KaitaiStruct):
        SEQ_FIELDS = ["virtual_address", "size"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['virtual_address']['start'] = self._io.pos()
            self.virtual_address = self._io.read_u4le()
            self._debug['virtual_address']['end'] = self._io.pos()
            self._debug['size']['start'] = self._io.pos()
            self.size = self._io.read_u4le()
            self._debug['size']['end'] = self._io.pos()


    class Section(KaitaiStruct):
        SEQ_FIELDS = ["name", "virtual_size", "virtual_address", "size_of_raw_data", "pointer_to_raw_data", "pointer_to_relocations", "pointer_to_linenumbers", "num_relocations", "num_linenumbers", "characteristics"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['name']['start'] = self._io.pos()
            self.name = (KaitaiStream.bytes_strip_right(self._io.read_bytes(8), 0)).decode(u"UTF-8")
            self._debug['name']['end'] = self._io.pos()
            self._debug['virtual_size']['start'] = self._io.pos()
            self.virtual_size = self._io.read_u4le()
            self._debug['virtual_size']['end'] = self._io.pos()
            self._debug['virtual_address']['start'] = self._io.pos()
            self.virtual_address = self._io.read_u4le()
            self._debug['virtual_address']['end'] = self._io.pos()
            self._debug['size_of_raw_data']['start'] = self._io.pos()
            self.size_of_raw_data = self._io.read_u4le()
            self._debug['size_of_raw_data']['end'] = self._io.pos()
            self._debug['pointer_to_raw_data']['start'] = self._io.pos()
            self.pointer_to_raw_data = self._io.read_u4le()
            self._debug['pointer_to_raw_data']['end'] = self._io.pos()
            self._debug['pointer_to_relocations']['start'] = self._io.pos()
            self.pointer_to_relocations = self._io.read_u4le()
            self._debug['pointer_to_relocations']['end'] = self._io.pos()
            self._debug['pointer_to_linenumbers']['start'] = self._io.pos()
            self.pointer_to_linenumbers = self._io.read_u4le()
            self._debug['pointer_to_linenumbers']['end'] = self._io.pos()
            self._debug['num_relocations']['start'] = self._io.pos()
            self.num_relocations = self._io.read_u2le()
            self._debug['num_relocations']['end'] = self._io.pos()
            self._debug['num_linenumbers']['start'] = self._io.pos()
            self.num_linenumbers = self._io.read_u2le()
            self._debug['num_linenumbers']['end'] = self._io.pos()
            self._debug['characteristics']['start'] = self._io.pos()
            self.characteristics = self._io.read_u4le()
            self._debug['characteristics']['end'] = self._io.pos()

        @property
        def body(self):
            if hasattr(self, '_m_body'):
                return self._m_body if hasattr(self, '_m_body') else None

            _pos = self._io.pos()
            self._io.seek(((self.pointer_to_raw_data - self._root.te_hdr.stripped_size) + self._root.te_hdr._io.size()))
            self._debug['_m_body']['start'] = self._io.pos()
            self._m_body = self._io.read_bytes(self.size_of_raw_data)
            self._debug['_m_body']['end'] = self._io.pos()
            self._io.seek(_pos)
            return self._m_body if hasattr(self, '_m_body') else None



