# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
import collections


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

import rtp_packet
class Rtpdump(KaitaiStruct):
    """rtpdump is a format used by rtptools to record and replay
    rtp data from network capture.
    
    .. seealso::
       Source - https://chromium.googlesource.com/external/webrtc/stable/talk/+/master/media/base/rtpdump.h
    """
    SEQ_FIELDS = ["file_header", "packets"]
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._debug = collections.defaultdict(dict)

    def _read(self):
        self._debug['file_header']['start'] = self._io.pos()
        self.file_header = self._root.HeaderT(self._io, self, self._root)
        self.file_header._read()
        self._debug['file_header']['end'] = self._io.pos()
        self._debug['packets']['start'] = self._io.pos()
        self.packets = []
        i = 0
        while not self._io.is_eof():
            if not 'arr' in self._debug['packets']:
                self._debug['packets']['arr'] = []
            self._debug['packets']['arr'].append({'start': self._io.pos()})
            _t_packets = self._root.PacketT(self._io, self, self._root)
            _t_packets._read()
            self.packets.append(_t_packets)
            self._debug['packets']['arr'][len(self.packets) - 1]['end'] = self._io.pos()
            i += 1

        self._debug['packets']['end'] = self._io.pos()

    class HeaderT(KaitaiStruct):
        SEQ_FIELDS = ["shebang", "space", "ip", "port", "start_sec", "start_usec", "ip2", "port2", "padding"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['shebang']['start'] = self._io.pos()
            self.shebang = self._io.read_bytes(12)
            self._debug['shebang']['end'] = self._io.pos()
            if not self.shebang == b"\x23\x21\x72\x74\x70\x70\x6C\x61\x79\x31\x2E\x30":
                raise kaitaistruct.ValidationNotEqualError(b"\x23\x21\x72\x74\x70\x70\x6C\x61\x79\x31\x2E\x30", self.shebang, self._io, u"/types/header_t/seq/0")
            self._debug['space']['start'] = self._io.pos()
            self.space = self._io.read_bytes(1)
            self._debug['space']['end'] = self._io.pos()
            if not self.space == b"\x20":
                raise kaitaistruct.ValidationNotEqualError(b"\x20", self.space, self._io, u"/types/header_t/seq/1")
            self._debug['ip']['start'] = self._io.pos()
            self.ip = (self._io.read_bytes_term(47, False, True, True)).decode(u"ascii")
            self._debug['ip']['end'] = self._io.pos()
            self._debug['port']['start'] = self._io.pos()
            self.port = (self._io.read_bytes_term(10, False, True, True)).decode(u"ascii")
            self._debug['port']['end'] = self._io.pos()
            self._debug['start_sec']['start'] = self._io.pos()
            self.start_sec = self._io.read_u4be()
            self._debug['start_sec']['end'] = self._io.pos()
            self._debug['start_usec']['start'] = self._io.pos()
            self.start_usec = self._io.read_u4be()
            self._debug['start_usec']['end'] = self._io.pos()
            self._debug['ip2']['start'] = self._io.pos()
            self.ip2 = self._io.read_u4be()
            self._debug['ip2']['end'] = self._io.pos()
            self._debug['port2']['start'] = self._io.pos()
            self.port2 = self._io.read_u2be()
            self._debug['port2']['end'] = self._io.pos()
            self._debug['padding']['start'] = self._io.pos()
            self.padding = self._io.read_u2be()
            self._debug['padding']['end'] = self._io.pos()


    class PacketT(KaitaiStruct):
        SEQ_FIELDS = ["length", "len_body", "packet_usec", "body"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['length']['start'] = self._io.pos()
            self.length = self._io.read_u2be()
            self._debug['length']['end'] = self._io.pos()
            self._debug['len_body']['start'] = self._io.pos()
            self.len_body = self._io.read_u2be()
            self._debug['len_body']['end'] = self._io.pos()
            self._debug['packet_usec']['start'] = self._io.pos()
            self.packet_usec = self._io.read_u4be()
            self._debug['packet_usec']['end'] = self._io.pos()
            self._debug['body']['start'] = self._io.pos()
            self._raw_body = self._io.read_bytes(self.len_body)
            _io__raw_body = KaitaiStream(BytesIO(self._raw_body))
            self.body = rtp_packet.RtpPacket(_io__raw_body)
            self.body._read()
            self._debug['body']['end'] = self._io.pos()



