# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
import collections
from enum import Enum


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

class BitcoinTransaction(KaitaiStruct):
    """
    .. seealso::
       Source - https://bitcoin.org/en/developer-guide#transactions
       https://en.bitcoin.it/wiki/Transaction
    """
    SEQ_FIELDS = ["version", "num_vins", "vins", "num_vouts", "vouts", "locktime"]
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._debug = collections.defaultdict(dict)

    def _read(self):
        self._debug['version']['start'] = self._io.pos()
        self.version = self._io.read_u4le()
        self._debug['version']['end'] = self._io.pos()
        self._debug['num_vins']['start'] = self._io.pos()
        self.num_vins = self._io.read_u1()
        self._debug['num_vins']['end'] = self._io.pos()
        self._debug['vins']['start'] = self._io.pos()
        self.vins = [None] * (self.num_vins)
        for i in range(self.num_vins):
            if not 'arr' in self._debug['vins']:
                self._debug['vins']['arr'] = []
            self._debug['vins']['arr'].append({'start': self._io.pos()})
            _t_vins = BitcoinTransaction.Vin(self._io, self, self._root)
            _t_vins._read()
            self.vins[i] = _t_vins
            self._debug['vins']['arr'][i]['end'] = self._io.pos()

        self._debug['vins']['end'] = self._io.pos()
        self._debug['num_vouts']['start'] = self._io.pos()
        self.num_vouts = self._io.read_u1()
        self._debug['num_vouts']['end'] = self._io.pos()
        self._debug['vouts']['start'] = self._io.pos()
        self.vouts = [None] * (self.num_vouts)
        for i in range(self.num_vouts):
            if not 'arr' in self._debug['vouts']:
                self._debug['vouts']['arr'] = []
            self._debug['vouts']['arr'].append({'start': self._io.pos()})
            _t_vouts = BitcoinTransaction.Vout(self._io, self, self._root)
            _t_vouts._read()
            self.vouts[i] = _t_vouts
            self._debug['vouts']['arr'][i]['end'] = self._io.pos()

        self._debug['vouts']['end'] = self._io.pos()
        self._debug['locktime']['start'] = self._io.pos()
        self.locktime = self._io.read_u4le()
        self._debug['locktime']['end'] = self._io.pos()

    class Vin(KaitaiStruct):
        SEQ_FIELDS = ["txid", "output_id", "len_script", "script_sig", "end_of_vin"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['txid']['start'] = self._io.pos()
            self.txid = self._io.read_bytes(32)
            self._debug['txid']['end'] = self._io.pos()
            self._debug['output_id']['start'] = self._io.pos()
            self.output_id = self._io.read_u4le()
            self._debug['output_id']['end'] = self._io.pos()
            self._debug['len_script']['start'] = self._io.pos()
            self.len_script = self._io.read_u1()
            self._debug['len_script']['end'] = self._io.pos()
            self._debug['script_sig']['start'] = self._io.pos()
            self._raw_script_sig = self._io.read_bytes(self.len_script)
            _io__raw_script_sig = KaitaiStream(BytesIO(self._raw_script_sig))
            self.script_sig = BitcoinTransaction.Vin.ScriptSignature(_io__raw_script_sig, self, self._root)
            self.script_sig._read()
            self._debug['script_sig']['end'] = self._io.pos()
            self._debug['end_of_vin']['start'] = self._io.pos()
            self.end_of_vin = self._io.read_bytes(4)
            self._debug['end_of_vin']['end'] = self._io.pos()
            if not self.end_of_vin == b"\xFF\xFF\xFF\xFF":
                raise kaitaistruct.ValidationNotEqualError(b"\xFF\xFF\xFF\xFF", self.end_of_vin, self._io, u"/types/vin/seq/4")

        class ScriptSignature(KaitaiStruct):

            class SighashType(Enum):
                sighash_all = 1
                sighash_none = 2
                sighash_single = 3
                sighash_anyonecanpay = 80
            SEQ_FIELDS = ["len_sig_stack", "der_sig", "sig_type", "len_pubkey_stack", "pubkey"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['len_sig_stack']['start'] = self._io.pos()
                self.len_sig_stack = self._io.read_u1()
                self._debug['len_sig_stack']['end'] = self._io.pos()
                self._debug['der_sig']['start'] = self._io.pos()
                self.der_sig = BitcoinTransaction.Vin.ScriptSignature.DerSignature(self._io, self, self._root)
                self.der_sig._read()
                self._debug['der_sig']['end'] = self._io.pos()
                self._debug['sig_type']['start'] = self._io.pos()
                self.sig_type = KaitaiStream.resolve_enum(BitcoinTransaction.Vin.ScriptSignature.SighashType, self._io.read_u1())
                self._debug['sig_type']['end'] = self._io.pos()
                self._debug['len_pubkey_stack']['start'] = self._io.pos()
                self.len_pubkey_stack = self._io.read_u1()
                self._debug['len_pubkey_stack']['end'] = self._io.pos()
                self._debug['pubkey']['start'] = self._io.pos()
                self.pubkey = BitcoinTransaction.Vin.ScriptSignature.PublicKey(self._io, self, self._root)
                self.pubkey._read()
                self._debug['pubkey']['end'] = self._io.pos()

            class DerSignature(KaitaiStruct):
                SEQ_FIELDS = ["sequence", "len_sig", "sep_1", "len_sig_r", "sig_r", "sep_2", "len_sig_s", "sig_s"]
                def __init__(self, _io, _parent=None, _root=None):
                    self._io = _io
                    self._parent = _parent
                    self._root = _root if _root else self
                    self._debug = collections.defaultdict(dict)

                def _read(self):
                    self._debug['sequence']['start'] = self._io.pos()
                    self.sequence = self._io.read_bytes(1)
                    self._debug['sequence']['end'] = self._io.pos()
                    if not self.sequence == b"\x30":
                        raise kaitaistruct.ValidationNotEqualError(b"\x30", self.sequence, self._io, u"/types/vin/types/script_signature/types/der_signature/seq/0")
                    self._debug['len_sig']['start'] = self._io.pos()
                    self.len_sig = self._io.read_u1()
                    self._debug['len_sig']['end'] = self._io.pos()
                    self._debug['sep_1']['start'] = self._io.pos()
                    self.sep_1 = self._io.read_bytes(1)
                    self._debug['sep_1']['end'] = self._io.pos()
                    if not self.sep_1 == b"\x02":
                        raise kaitaistruct.ValidationNotEqualError(b"\x02", self.sep_1, self._io, u"/types/vin/types/script_signature/types/der_signature/seq/2")
                    self._debug['len_sig_r']['start'] = self._io.pos()
                    self.len_sig_r = self._io.read_u1()
                    self._debug['len_sig_r']['end'] = self._io.pos()
                    self._debug['sig_r']['start'] = self._io.pos()
                    self.sig_r = self._io.read_bytes(self.len_sig_r)
                    self._debug['sig_r']['end'] = self._io.pos()
                    self._debug['sep_2']['start'] = self._io.pos()
                    self.sep_2 = self._io.read_bytes(1)
                    self._debug['sep_2']['end'] = self._io.pos()
                    if not self.sep_2 == b"\x02":
                        raise kaitaistruct.ValidationNotEqualError(b"\x02", self.sep_2, self._io, u"/types/vin/types/script_signature/types/der_signature/seq/5")
                    self._debug['len_sig_s']['start'] = self._io.pos()
                    self.len_sig_s = self._io.read_u1()
                    self._debug['len_sig_s']['end'] = self._io.pos()
                    self._debug['sig_s']['start'] = self._io.pos()
                    self.sig_s = self._io.read_bytes(self.len_sig_s)
                    self._debug['sig_s']['end'] = self._io.pos()


            class PublicKey(KaitaiStruct):
                SEQ_FIELDS = ["type", "x", "y"]
                def __init__(self, _io, _parent=None, _root=None):
                    self._io = _io
                    self._parent = _parent
                    self._root = _root if _root else self
                    self._debug = collections.defaultdict(dict)

                def _read(self):
                    self._debug['type']['start'] = self._io.pos()
                    self.type = self._io.read_u1()
                    self._debug['type']['end'] = self._io.pos()
                    self._debug['x']['start'] = self._io.pos()
                    self.x = self._io.read_bytes(32)
                    self._debug['x']['end'] = self._io.pos()
                    self._debug['y']['start'] = self._io.pos()
                    self.y = self._io.read_bytes(32)
                    self._debug['y']['end'] = self._io.pos()




    class Vout(KaitaiStruct):
        SEQ_FIELDS = ["amount", "len_script", "script_pub_key"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['amount']['start'] = self._io.pos()
            self.amount = self._io.read_u8le()
            self._debug['amount']['end'] = self._io.pos()
            self._debug['len_script']['start'] = self._io.pos()
            self.len_script = self._io.read_u1()
            self._debug['len_script']['end'] = self._io.pos()
            self._debug['script_pub_key']['start'] = self._io.pos()
            self.script_pub_key = self._io.read_bytes(self.len_script)
            self._debug['script_pub_key']['end'] = self._io.pos()



