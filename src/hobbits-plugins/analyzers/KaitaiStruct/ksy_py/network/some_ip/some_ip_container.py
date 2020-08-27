# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
import collections


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

import some_ip
class SomeIpContainer(KaitaiStruct):
    SEQ_FIELDS = ["some_ip_packages"]
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._debug = collections.defaultdict(dict)

    def _read(self):
        self._debug['some_ip_packages']['start'] = self._io.pos()
        self.some_ip_packages = []
        i = 0
        while not self._io.is_eof():
            if not 'arr' in self._debug['some_ip_packages']:
                self._debug['some_ip_packages']['arr'] = []
            self._debug['some_ip_packages']['arr'].append({'start': self._io.pos()})
            _t_some_ip_packages = some_ip.SomeIp(self._io)
            _t_some_ip_packages._read()
            self.some_ip_packages.append(_t_some_ip_packages)
            self._debug['some_ip_packages']['arr'][len(self.some_ip_packages) - 1]['end'] = self._io.pos()
            i += 1

        self._debug['some_ip_packages']['end'] = self._io.pos()


