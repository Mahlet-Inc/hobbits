# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
from enum import Enum
import collections


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

class SudoersTs(KaitaiStruct):
    """This spec can be used to parse sudo time stamp files located in directories
    such as /run/sudo/ts/$USER or /var/lib/sudo/ts/$USER.
    
    .. seealso::
       Source - https://www.sudo.ws/man/1.8.27/sudoers_timestamp.man.html
    """

    class TsType(Enum):
        global = 1
        tty = 2
        ppid = 3
        lockexcl = 4
    SEQ_FIELDS = ["records"]
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._debug = collections.defaultdict(dict)

    def _read(self):
        self._debug['records']['start'] = self._io.pos()
        self.records = []
        i = 0
        while not self._io.is_eof():
            if not 'arr' in self._debug['records']:
                self._debug['records']['arr'] = []
            self._debug['records']['arr'].append({'start': self._io.pos()})
            _t_records = SudoersTs.Record(self._io, self, self._root)
            _t_records._read()
            self.records.append(_t_records)
            self._debug['records']['arr'][len(self.records) - 1]['end'] = self._io.pos()
            i += 1

        self._debug['records']['end'] = self._io.pos()

    class RecordV2(KaitaiStruct):
        SEQ_FIELDS = ["type", "flags", "auth_uid", "sid", "start_time", "ts", "ttydev", "ppid"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['type']['start'] = self._io.pos()
            self.type = KaitaiStream.resolve_enum(SudoersTs.TsType, self._io.read_u2le())
            self._debug['type']['end'] = self._io.pos()
            self._debug['flags']['start'] = self._io.pos()
            self.flags = SudoersTs.TsFlag(self._io, self, self._root)
            self.flags._read()
            self._debug['flags']['end'] = self._io.pos()
            self._debug['auth_uid']['start'] = self._io.pos()
            self.auth_uid = self._io.read_u4le()
            self._debug['auth_uid']['end'] = self._io.pos()
            self._debug['sid']['start'] = self._io.pos()
            self.sid = self._io.read_u4le()
            self._debug['sid']['end'] = self._io.pos()
            self._debug['start_time']['start'] = self._io.pos()
            self.start_time = SudoersTs.Timespec(self._io, self, self._root)
            self.start_time._read()
            self._debug['start_time']['end'] = self._io.pos()
            self._debug['ts']['start'] = self._io.pos()
            self.ts = SudoersTs.Timespec(self._io, self, self._root)
            self.ts._read()
            self._debug['ts']['end'] = self._io.pos()
            if self.type == SudoersTs.TsType.tty:
                self._debug['ttydev']['start'] = self._io.pos()
                self.ttydev = self._io.read_u4le()
                self._debug['ttydev']['end'] = self._io.pos()

            if self.type == SudoersTs.TsType.ppid:
                self._debug['ppid']['start'] = self._io.pos()
                self.ppid = self._io.read_u4le()
                self._debug['ppid']['end'] = self._io.pos()



    class TsFlag(KaitaiStruct):
        SEQ_FIELDS = ["reserved0", "anyuid", "disabled", "reserved1"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['reserved0']['start'] = self._io.pos()
            self.reserved0 = self._io.read_bits_int_be(6)
            self._debug['reserved0']['end'] = self._io.pos()
            self._debug['anyuid']['start'] = self._io.pos()
            self.anyuid = self._io.read_bits_int_be(1) != 0
            self._debug['anyuid']['end'] = self._io.pos()
            self._debug['disabled']['start'] = self._io.pos()
            self.disabled = self._io.read_bits_int_be(1) != 0
            self._debug['disabled']['end'] = self._io.pos()
            self._debug['reserved1']['start'] = self._io.pos()
            self.reserved1 = self._io.read_bits_int_be(8)
            self._debug['reserved1']['end'] = self._io.pos()


    class RecordV1(KaitaiStruct):
        SEQ_FIELDS = ["type", "flags", "auth_uid", "sid", "ts", "ttydev", "ppid"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['type']['start'] = self._io.pos()
            self.type = KaitaiStream.resolve_enum(SudoersTs.TsType, self._io.read_u2le())
            self._debug['type']['end'] = self._io.pos()
            self._debug['flags']['start'] = self._io.pos()
            self.flags = SudoersTs.TsFlag(self._io, self, self._root)
            self.flags._read()
            self._debug['flags']['end'] = self._io.pos()
            self._debug['auth_uid']['start'] = self._io.pos()
            self.auth_uid = self._io.read_u4le()
            self._debug['auth_uid']['end'] = self._io.pos()
            self._debug['sid']['start'] = self._io.pos()
            self.sid = self._io.read_u4le()
            self._debug['sid']['end'] = self._io.pos()
            self._debug['ts']['start'] = self._io.pos()
            self.ts = SudoersTs.Timespec(self._io, self, self._root)
            self.ts._read()
            self._debug['ts']['end'] = self._io.pos()
            if self.type == SudoersTs.TsType.tty:
                self._debug['ttydev']['start'] = self._io.pos()
                self.ttydev = self._io.read_u4le()
                self._debug['ttydev']['end'] = self._io.pos()

            if self.type == SudoersTs.TsType.ppid:
                self._debug['ppid']['start'] = self._io.pos()
                self.ppid = self._io.read_u4le()
                self._debug['ppid']['end'] = self._io.pos()



    class Timespec(KaitaiStruct):
        SEQ_FIELDS = ["sec", "nsec"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['sec']['start'] = self._io.pos()
            self.sec = self._io.read_s8le()
            self._debug['sec']['end'] = self._io.pos()
            self._debug['nsec']['start'] = self._io.pos()
            self.nsec = self._io.read_s8le()
            self._debug['nsec']['end'] = self._io.pos()


    class Record(KaitaiStruct):
        SEQ_FIELDS = ["version", "len_record", "payload"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['version']['start'] = self._io.pos()
            self.version = self._io.read_u2le()
            self._debug['version']['end'] = self._io.pos()
            self._debug['len_record']['start'] = self._io.pos()
            self.len_record = self._io.read_u2le()
            self._debug['len_record']['end'] = self._io.pos()
            self._debug['payload']['start'] = self._io.pos()
            _on = self.version
            if _on == 1:
                self._raw_payload = self._io.read_bytes((self.len_record - 4))
                _io__raw_payload = KaitaiStream(BytesIO(self._raw_payload))
                self.payload = SudoersTs.RecordV1(_io__raw_payload, self, self._root)
                self.payload._read()
            elif _on == 2:
                self._raw_payload = self._io.read_bytes((self.len_record - 4))
                _io__raw_payload = KaitaiStream(BytesIO(self._raw_payload))
                self.payload = SudoersTs.RecordV2(_io__raw_payload, self, self._root)
                self.payload._read()
            else:
                self.payload = self._io.read_bytes((self.len_record - 4))
            self._debug['payload']['end'] = self._io.pos()



