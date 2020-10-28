# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
from enum import Enum
import collections


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

class DnsPacket(KaitaiStruct):
    """(No support for Auth-Name + Add-Name for simplicity)
    """

    class ClassType(Enum):
        in_class = 1
        cs = 2
        ch = 3
        hs = 4

    class TypeType(Enum):
        a = 1
        ns = 2
        md = 3
        mf = 4
        cname = 5
        soa = 6
        mb = 7
        mg = 8
        mr = 9
        null = 10
        wks = 11
        ptr = 12
        hinfo = 13
        minfo = 14
        mx = 15
        txt = 16
        aaaa = 28
        srv = 33
    SEQ_FIELDS = ["transaction_id", "flags", "qdcount", "ancount", "nscount", "arcount", "queries", "answers", "authorities", "additionals"]
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._debug = collections.defaultdict(dict)

    def _read(self):
        self._debug['transaction_id']['start'] = self._io.pos()
        self.transaction_id = self._io.read_u2be()
        self._debug['transaction_id']['end'] = self._io.pos()
        self._debug['flags']['start'] = self._io.pos()
        self.flags = DnsPacket.PacketFlags(self._io, self, self._root)
        self.flags._read()
        self._debug['flags']['end'] = self._io.pos()
        if self.flags.is_opcode_valid:
            self._debug['qdcount']['start'] = self._io.pos()
            self.qdcount = self._io.read_u2be()
            self._debug['qdcount']['end'] = self._io.pos()

        if self.flags.is_opcode_valid:
            self._debug['ancount']['start'] = self._io.pos()
            self.ancount = self._io.read_u2be()
            self._debug['ancount']['end'] = self._io.pos()

        if self.flags.is_opcode_valid:
            self._debug['nscount']['start'] = self._io.pos()
            self.nscount = self._io.read_u2be()
            self._debug['nscount']['end'] = self._io.pos()

        if self.flags.is_opcode_valid:
            self._debug['arcount']['start'] = self._io.pos()
            self.arcount = self._io.read_u2be()
            self._debug['arcount']['end'] = self._io.pos()

        if self.flags.is_opcode_valid:
            self._debug['queries']['start'] = self._io.pos()
            self.queries = [None] * (self.qdcount)
            for i in range(self.qdcount):
                if not 'arr' in self._debug['queries']:
                    self._debug['queries']['arr'] = []
                self._debug['queries']['arr'].append({'start': self._io.pos()})
                _t_queries = DnsPacket.Query(self._io, self, self._root)
                _t_queries._read()
                self.queries[i] = _t_queries
                self._debug['queries']['arr'][i]['end'] = self._io.pos()

            self._debug['queries']['end'] = self._io.pos()

        if self.flags.is_opcode_valid:
            self._debug['answers']['start'] = self._io.pos()
            self.answers = [None] * (self.ancount)
            for i in range(self.ancount):
                if not 'arr' in self._debug['answers']:
                    self._debug['answers']['arr'] = []
                self._debug['answers']['arr'].append({'start': self._io.pos()})
                _t_answers = DnsPacket.Answer(self._io, self, self._root)
                _t_answers._read()
                self.answers[i] = _t_answers
                self._debug['answers']['arr'][i]['end'] = self._io.pos()

            self._debug['answers']['end'] = self._io.pos()

        if self.flags.is_opcode_valid:
            self._debug['authorities']['start'] = self._io.pos()
            self.authorities = [None] * (self.nscount)
            for i in range(self.nscount):
                if not 'arr' in self._debug['authorities']:
                    self._debug['authorities']['arr'] = []
                self._debug['authorities']['arr'].append({'start': self._io.pos()})
                _t_authorities = DnsPacket.Answer(self._io, self, self._root)
                _t_authorities._read()
                self.authorities[i] = _t_authorities
                self._debug['authorities']['arr'][i]['end'] = self._io.pos()

            self._debug['authorities']['end'] = self._io.pos()

        if self.flags.is_opcode_valid:
            self._debug['additionals']['start'] = self._io.pos()
            self.additionals = [None] * (self.arcount)
            for i in range(self.arcount):
                if not 'arr' in self._debug['additionals']:
                    self._debug['additionals']['arr'] = []
                self._debug['additionals']['arr'].append({'start': self._io.pos()})
                _t_additionals = DnsPacket.Answer(self._io, self, self._root)
                _t_additionals._read()
                self.additionals[i] = _t_additionals
                self._debug['additionals']['arr'][i]['end'] = self._io.pos()

            self._debug['additionals']['end'] = self._io.pos()


    class MxInfo(KaitaiStruct):
        SEQ_FIELDS = ["preference", "mx"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['preference']['start'] = self._io.pos()
            self.preference = self._io.read_u2be()
            self._debug['preference']['end'] = self._io.pos()
            self._debug['mx']['start'] = self._io.pos()
            self.mx = DnsPacket.DomainName(self._io, self, self._root)
            self.mx._read()
            self._debug['mx']['end'] = self._io.pos()


    class PointerStruct(KaitaiStruct):
        SEQ_FIELDS = ["value"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['value']['start'] = self._io.pos()
            self.value = self._io.read_u1()
            self._debug['value']['end'] = self._io.pos()

        @property
        def contents(self):
            if hasattr(self, '_m_contents'):
                return self._m_contents if hasattr(self, '_m_contents') else None

            io = self._root._io
            _pos = io.pos()
            io.seek((self.value + ((self._parent.length - 192) << 8)))
            self._debug['_m_contents']['start'] = io.pos()
            self._m_contents = DnsPacket.DomainName(io, self, self._root)
            self._m_contents._read()
            self._debug['_m_contents']['end'] = io.pos()
            io.seek(_pos)
            return self._m_contents if hasattr(self, '_m_contents') else None


    class Label(KaitaiStruct):
        SEQ_FIELDS = ["length", "pointer", "name"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['length']['start'] = self._io.pos()
            self.length = self._io.read_u1()
            self._debug['length']['end'] = self._io.pos()
            if self.is_pointer:
                self._debug['pointer']['start'] = self._io.pos()
                self.pointer = DnsPacket.PointerStruct(self._io, self, self._root)
                self.pointer._read()
                self._debug['pointer']['end'] = self._io.pos()

            if not (self.is_pointer):
                self._debug['name']['start'] = self._io.pos()
                self.name = (self._io.read_bytes(self.length)).decode(u"utf-8")
                self._debug['name']['end'] = self._io.pos()


        @property
        def is_pointer(self):
            if hasattr(self, '_m_is_pointer'):
                return self._m_is_pointer if hasattr(self, '_m_is_pointer') else None

            self._m_is_pointer = self.length >= 192
            return self._m_is_pointer if hasattr(self, '_m_is_pointer') else None


    class Query(KaitaiStruct):
        SEQ_FIELDS = ["name", "type", "query_class"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['name']['start'] = self._io.pos()
            self.name = DnsPacket.DomainName(self._io, self, self._root)
            self.name._read()
            self._debug['name']['end'] = self._io.pos()
            self._debug['type']['start'] = self._io.pos()
            self.type = KaitaiStream.resolve_enum(DnsPacket.TypeType, self._io.read_u2be())
            self._debug['type']['end'] = self._io.pos()
            self._debug['query_class']['start'] = self._io.pos()
            self.query_class = KaitaiStream.resolve_enum(DnsPacket.ClassType, self._io.read_u2be())
            self._debug['query_class']['end'] = self._io.pos()


    class DomainName(KaitaiStruct):
        SEQ_FIELDS = ["name"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['name']['start'] = self._io.pos()
            self.name = []
            i = 0
            while True:
                if not 'arr' in self._debug['name']:
                    self._debug['name']['arr'] = []
                self._debug['name']['arr'].append({'start': self._io.pos()})
                _t_name = DnsPacket.Label(self._io, self, self._root)
                _t_name._read()
                _ = _t_name
                self.name.append(_)
                self._debug['name']['arr'][len(self.name) - 1]['end'] = self._io.pos()
                if  ((_.length == 0) or (_.length >= 192)) :
                    break
                i += 1
            self._debug['name']['end'] = self._io.pos()


    class AddressV6(KaitaiStruct):
        SEQ_FIELDS = ["ip_v6"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['ip_v6']['start'] = self._io.pos()
            self.ip_v6 = self._io.read_bytes(16)
            self._debug['ip_v6']['end'] = self._io.pos()


    class Service(KaitaiStruct):
        SEQ_FIELDS = ["priority", "weight", "port", "target"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['priority']['start'] = self._io.pos()
            self.priority = self._io.read_u2be()
            self._debug['priority']['end'] = self._io.pos()
            self._debug['weight']['start'] = self._io.pos()
            self.weight = self._io.read_u2be()
            self._debug['weight']['end'] = self._io.pos()
            self._debug['port']['start'] = self._io.pos()
            self.port = self._io.read_u2be()
            self._debug['port']['end'] = self._io.pos()
            self._debug['target']['start'] = self._io.pos()
            self.target = DnsPacket.DomainName(self._io, self, self._root)
            self.target._read()
            self._debug['target']['end'] = self._io.pos()


    class Txt(KaitaiStruct):
        SEQ_FIELDS = ["length", "text"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['length']['start'] = self._io.pos()
            self.length = self._io.read_u1()
            self._debug['length']['end'] = self._io.pos()
            self._debug['text']['start'] = self._io.pos()
            self.text = (self._io.read_bytes(self.length)).decode(u"utf-8")
            self._debug['text']['end'] = self._io.pos()


    class TxtBody(KaitaiStruct):
        SEQ_FIELDS = ["data"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['data']['start'] = self._io.pos()
            self.data = []
            i = 0
            while not self._io.is_eof():
                if not 'arr' in self._debug['data']:
                    self._debug['data']['arr'] = []
                self._debug['data']['arr'].append({'start': self._io.pos()})
                _t_data = DnsPacket.Txt(self._io, self, self._root)
                _t_data._read()
                self.data.append(_t_data)
                self._debug['data']['arr'][len(self.data) - 1]['end'] = self._io.pos()
                i += 1

            self._debug['data']['end'] = self._io.pos()


    class Address(KaitaiStruct):
        SEQ_FIELDS = ["ip"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['ip']['start'] = self._io.pos()
            self.ip = self._io.read_bytes(4)
            self._debug['ip']['end'] = self._io.pos()


    class Answer(KaitaiStruct):
        SEQ_FIELDS = ["name", "type", "answer_class", "ttl", "rdlength", "payload"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['name']['start'] = self._io.pos()
            self.name = DnsPacket.DomainName(self._io, self, self._root)
            self.name._read()
            self._debug['name']['end'] = self._io.pos()
            self._debug['type']['start'] = self._io.pos()
            self.type = KaitaiStream.resolve_enum(DnsPacket.TypeType, self._io.read_u2be())
            self._debug['type']['end'] = self._io.pos()
            self._debug['answer_class']['start'] = self._io.pos()
            self.answer_class = KaitaiStream.resolve_enum(DnsPacket.ClassType, self._io.read_u2be())
            self._debug['answer_class']['end'] = self._io.pos()
            self._debug['ttl']['start'] = self._io.pos()
            self.ttl = self._io.read_s4be()
            self._debug['ttl']['end'] = self._io.pos()
            self._debug['rdlength']['start'] = self._io.pos()
            self.rdlength = self._io.read_u2be()
            self._debug['rdlength']['end'] = self._io.pos()
            self._debug['payload']['start'] = self._io.pos()
            _on = self.type
            if _on == DnsPacket.TypeType.srv:
                self._raw_payload = self._io.read_bytes(self.rdlength)
                _io__raw_payload = KaitaiStream(BytesIO(self._raw_payload))
                self.payload = DnsPacket.Service(_io__raw_payload, self, self._root)
                self.payload._read()
            elif _on == DnsPacket.TypeType.a:
                self._raw_payload = self._io.read_bytes(self.rdlength)
                _io__raw_payload = KaitaiStream(BytesIO(self._raw_payload))
                self.payload = DnsPacket.Address(_io__raw_payload, self, self._root)
                self.payload._read()
            elif _on == DnsPacket.TypeType.cname:
                self._raw_payload = self._io.read_bytes(self.rdlength)
                _io__raw_payload = KaitaiStream(BytesIO(self._raw_payload))
                self.payload = DnsPacket.DomainName(_io__raw_payload, self, self._root)
                self.payload._read()
            elif _on == DnsPacket.TypeType.ns:
                self._raw_payload = self._io.read_bytes(self.rdlength)
                _io__raw_payload = KaitaiStream(BytesIO(self._raw_payload))
                self.payload = DnsPacket.DomainName(_io__raw_payload, self, self._root)
                self.payload._read()
            elif _on == DnsPacket.TypeType.soa:
                self._raw_payload = self._io.read_bytes(self.rdlength)
                _io__raw_payload = KaitaiStream(BytesIO(self._raw_payload))
                self.payload = DnsPacket.AuthorityInfo(_io__raw_payload, self, self._root)
                self.payload._read()
            elif _on == DnsPacket.TypeType.mx:
                self._raw_payload = self._io.read_bytes(self.rdlength)
                _io__raw_payload = KaitaiStream(BytesIO(self._raw_payload))
                self.payload = DnsPacket.MxInfo(_io__raw_payload, self, self._root)
                self.payload._read()
            elif _on == DnsPacket.TypeType.txt:
                self._raw_payload = self._io.read_bytes(self.rdlength)
                _io__raw_payload = KaitaiStream(BytesIO(self._raw_payload))
                self.payload = DnsPacket.TxtBody(_io__raw_payload, self, self._root)
                self.payload._read()
            elif _on == DnsPacket.TypeType.ptr:
                self._raw_payload = self._io.read_bytes(self.rdlength)
                _io__raw_payload = KaitaiStream(BytesIO(self._raw_payload))
                self.payload = DnsPacket.DomainName(_io__raw_payload, self, self._root)
                self.payload._read()
            elif _on == DnsPacket.TypeType.aaaa:
                self._raw_payload = self._io.read_bytes(self.rdlength)
                _io__raw_payload = KaitaiStream(BytesIO(self._raw_payload))
                self.payload = DnsPacket.AddressV6(_io__raw_payload, self, self._root)
                self.payload._read()
            else:
                self.payload = self._io.read_bytes(self.rdlength)
            self._debug['payload']['end'] = self._io.pos()


    class PacketFlags(KaitaiStruct):
        SEQ_FIELDS = ["flag"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['flag']['start'] = self._io.pos()
            self.flag = self._io.read_u2be()
            self._debug['flag']['end'] = self._io.pos()

        @property
        def qr(self):
            if hasattr(self, '_m_qr'):
                return self._m_qr if hasattr(self, '_m_qr') else None

            self._m_qr = ((self.flag & 32768) >> 15)
            return self._m_qr if hasattr(self, '_m_qr') else None

        @property
        def ra(self):
            if hasattr(self, '_m_ra'):
                return self._m_ra if hasattr(self, '_m_ra') else None

            self._m_ra = ((self.flag & 128) >> 7)
            return self._m_ra if hasattr(self, '_m_ra') else None

        @property
        def tc(self):
            if hasattr(self, '_m_tc'):
                return self._m_tc if hasattr(self, '_m_tc') else None

            self._m_tc = ((self.flag & 512) >> 9)
            return self._m_tc if hasattr(self, '_m_tc') else None

        @property
        def is_opcode_valid(self):
            if hasattr(self, '_m_is_opcode_valid'):
                return self._m_is_opcode_valid if hasattr(self, '_m_is_opcode_valid') else None

            self._m_is_opcode_valid =  ((self.opcode == 0) or (self.opcode == 1) or (self.opcode == 2)) 
            return self._m_is_opcode_valid if hasattr(self, '_m_is_opcode_valid') else None

        @property
        def rcode(self):
            if hasattr(self, '_m_rcode'):
                return self._m_rcode if hasattr(self, '_m_rcode') else None

            self._m_rcode = ((self.flag & 15) >> 0)
            return self._m_rcode if hasattr(self, '_m_rcode') else None

        @property
        def opcode(self):
            if hasattr(self, '_m_opcode'):
                return self._m_opcode if hasattr(self, '_m_opcode') else None

            self._m_opcode = ((self.flag & 30720) >> 11)
            return self._m_opcode if hasattr(self, '_m_opcode') else None

        @property
        def aa(self):
            if hasattr(self, '_m_aa'):
                return self._m_aa if hasattr(self, '_m_aa') else None

            self._m_aa = ((self.flag & 1024) >> 10)
            return self._m_aa if hasattr(self, '_m_aa') else None

        @property
        def z(self):
            if hasattr(self, '_m_z'):
                return self._m_z if hasattr(self, '_m_z') else None

            self._m_z = ((self.flag & 64) >> 6)
            return self._m_z if hasattr(self, '_m_z') else None

        @property
        def rd(self):
            if hasattr(self, '_m_rd'):
                return self._m_rd if hasattr(self, '_m_rd') else None

            self._m_rd = ((self.flag & 256) >> 8)
            return self._m_rd if hasattr(self, '_m_rd') else None

        @property
        def cd(self):
            if hasattr(self, '_m_cd'):
                return self._m_cd if hasattr(self, '_m_cd') else None

            self._m_cd = ((self.flag & 16) >> 4)
            return self._m_cd if hasattr(self, '_m_cd') else None

        @property
        def ad(self):
            if hasattr(self, '_m_ad'):
                return self._m_ad if hasattr(self, '_m_ad') else None

            self._m_ad = ((self.flag & 32) >> 5)
            return self._m_ad if hasattr(self, '_m_ad') else None


    class AuthorityInfo(KaitaiStruct):
        SEQ_FIELDS = ["primary_ns", "resoponsible_mailbox", "serial", "refresh_interval", "retry_interval", "expire_limit", "min_ttl"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['primary_ns']['start'] = self._io.pos()
            self.primary_ns = DnsPacket.DomainName(self._io, self, self._root)
            self.primary_ns._read()
            self._debug['primary_ns']['end'] = self._io.pos()
            self._debug['resoponsible_mailbox']['start'] = self._io.pos()
            self.resoponsible_mailbox = DnsPacket.DomainName(self._io, self, self._root)
            self.resoponsible_mailbox._read()
            self._debug['resoponsible_mailbox']['end'] = self._io.pos()
            self._debug['serial']['start'] = self._io.pos()
            self.serial = self._io.read_u4be()
            self._debug['serial']['end'] = self._io.pos()
            self._debug['refresh_interval']['start'] = self._io.pos()
            self.refresh_interval = self._io.read_u4be()
            self._debug['refresh_interval']['end'] = self._io.pos()
            self._debug['retry_interval']['start'] = self._io.pos()
            self.retry_interval = self._io.read_u4be()
            self._debug['retry_interval']['end'] = self._io.pos()
            self._debug['expire_limit']['start'] = self._io.pos()
            self.expire_limit = self._io.read_u4be()
            self._debug['expire_limit']['end'] = self._io.pos()
            self._debug['min_ttl']['start'] = self._io.pos()
            self.min_ttl = self._io.read_u4be()
            self._debug['min_ttl']['end'] = self._io.pos()



