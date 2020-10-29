# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
from enum import Enum
import collections


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

class Websocket(KaitaiStruct):
    """The WebSocket protocol establishes a two-way communication channel via TCP.
    Messages are made up of one or more dataframes, and are delineated by
    frames with the `fin` bit set.
    """

    class Opcode(Enum):
        continuation = 0
        text = 1
        binary = 2
        reserved_3 = 3
        reserved_4 = 4
        reserved_5 = 5
        reserved_6 = 6
        reserved_7 = 7
        close = 8
        ping = 9
        pong = 10
        reserved_control_b = 11
        reserved_control_c = 12
        reserved_control_d = 13
        reserved_control_e = 14
        reserved_control_f = 15
    SEQ_FIELDS = ["initial_frame", "trailing_frames"]
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._debug = collections.defaultdict(dict)

    def _read(self):
        self._debug['initial_frame']['start'] = self._io.pos()
        self.initial_frame = Websocket.InitialFrame(self._io, self, self._root)
        self.initial_frame._read()
        self._debug['initial_frame']['end'] = self._io.pos()
        if self.initial_frame.header.finished != True:
            self._debug['trailing_frames']['start'] = self._io.pos()
            self.trailing_frames = []
            i = 0
            while True:
                if not 'arr' in self._debug['trailing_frames']:
                    self._debug['trailing_frames']['arr'] = []
                self._debug['trailing_frames']['arr'].append({'start': self._io.pos()})
                _t_trailing_frames = Websocket.Dataframe(self._io, self, self._root)
                _t_trailing_frames._read()
                _ = _t_trailing_frames
                self.trailing_frames.append(_)
                self._debug['trailing_frames']['arr'][len(self.trailing_frames) - 1]['end'] = self._io.pos()
                if _.header.finished:
                    break
                i += 1
            self._debug['trailing_frames']['end'] = self._io.pos()


    class FrameHeader(KaitaiStruct):
        SEQ_FIELDS = ["finished", "reserved", "opcode", "is_masked", "len_payload_primary", "len_payload_extended_1", "len_payload_extended_2", "mask_key"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['finished']['start'] = self._io.pos()
            self.finished = self._io.read_bits_int_be(1) != 0
            self._debug['finished']['end'] = self._io.pos()
            self._debug['reserved']['start'] = self._io.pos()
            self.reserved = self._io.read_bits_int_be(3)
            self._debug['reserved']['end'] = self._io.pos()
            self._debug['opcode']['start'] = self._io.pos()
            self.opcode = KaitaiStream.resolve_enum(Websocket.Opcode, self._io.read_bits_int_be(4))
            self._debug['opcode']['end'] = self._io.pos()
            self._debug['is_masked']['start'] = self._io.pos()
            self.is_masked = self._io.read_bits_int_be(1) != 0
            self._debug['is_masked']['end'] = self._io.pos()
            self._debug['len_payload_primary']['start'] = self._io.pos()
            self.len_payload_primary = self._io.read_bits_int_be(7)
            self._debug['len_payload_primary']['end'] = self._io.pos()
            self._io.align_to_byte()
            if self.len_payload_primary == 126:
                self._debug['len_payload_extended_1']['start'] = self._io.pos()
                self.len_payload_extended_1 = self._io.read_u2be()
                self._debug['len_payload_extended_1']['end'] = self._io.pos()

            if self.len_payload_primary == 127:
                self._debug['len_payload_extended_2']['start'] = self._io.pos()
                self.len_payload_extended_2 = self._io.read_u4be()
                self._debug['len_payload_extended_2']['end'] = self._io.pos()

            if self.is_masked:
                self._debug['mask_key']['start'] = self._io.pos()
                self.mask_key = self._io.read_u4be()
                self._debug['mask_key']['end'] = self._io.pos()


        @property
        def len_payload(self):
            if hasattr(self, '_m_len_payload'):
                return self._m_len_payload if hasattr(self, '_m_len_payload') else None

            self._m_len_payload = (self.len_payload_primary if self.len_payload_primary <= 125 else (self.len_payload_extended_1 if self.len_payload_primary == 126 else self.len_payload_extended_2))
            return self._m_len_payload if hasattr(self, '_m_len_payload') else None


    class InitialFrame(KaitaiStruct):
        SEQ_FIELDS = ["header", "payload_bytes", "payload_text"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['header']['start'] = self._io.pos()
            self.header = Websocket.FrameHeader(self._io, self, self._root)
            self.header._read()
            self._debug['header']['end'] = self._io.pos()
            if self.header.opcode != Websocket.Opcode.text:
                self._debug['payload_bytes']['start'] = self._io.pos()
                self.payload_bytes = self._io.read_bytes(self.header.len_payload)
                self._debug['payload_bytes']['end'] = self._io.pos()

            if self.header.opcode == Websocket.Opcode.text:
                self._debug['payload_text']['start'] = self._io.pos()
                self.payload_text = (self._io.read_bytes(self.header.len_payload)).decode(u"UTF-8")
                self._debug['payload_text']['end'] = self._io.pos()



    class Dataframe(KaitaiStruct):
        SEQ_FIELDS = ["header", "payload_bytes", "payload_text"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['header']['start'] = self._io.pos()
            self.header = Websocket.FrameHeader(self._io, self, self._root)
            self.header._read()
            self._debug['header']['end'] = self._io.pos()
            if self._root.initial_frame.header.opcode != Websocket.Opcode.text:
                self._debug['payload_bytes']['start'] = self._io.pos()
                self.payload_bytes = self._io.read_bytes(self.header.len_payload)
                self._debug['payload_bytes']['end'] = self._io.pos()

            if self._root.initial_frame.header.opcode == Websocket.Opcode.text:
                self._debug['payload_text']['start'] = self._io.pos()
                self.payload_text = (self._io.read_bytes(self.header.len_payload)).decode(u"UTF-8")
                self._debug['payload_text']['end'] = self._io.pos()




