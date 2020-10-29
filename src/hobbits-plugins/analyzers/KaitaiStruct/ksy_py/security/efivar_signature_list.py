# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
import collections


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

class EfivarSignatureList(KaitaiStruct):
    """Parse UEFI variables db and dbx that contain signatures, certificates and
    hashes. On a Linux system using UEFI, these variables are readable from
    /sys/firmware/efi/efivars/db-d719b2cb-3d3a-4596-a3bc-dad00e67656f,
    /sys/firmware/efi/efivars/dbDefault-8be4df61-93ca-11d2-aa0d-00e098032b8c,
    /sys/firmware/efi/efivars/dbx-d719b2cb-3d3a-4596-a3bc-dad00e67656f and
    /sys/firmware/efi/efivars/dbxDefault-8be4df61-93ca-11d2-aa0d-00e098032b8c.
    ("d719b2cb-3d3a-4596-a3bc-dad00e67656f" is defined as
    EFI_IMAGE_SECURITY_DATABASE_GUID and "8be4df61-93ca-11d2-aa0d-00e098032b8c"
    as EFI_GLOBAL_VARIABLE).
    Each file contains an EFI attribute (32-bit integer) followed by a list of
    EFI_SIGNATURE_LIST structures.
    
    .. seealso::
       Source - https://uefi.org/sites/default/files/resources/UEFI_Spec_2_8_final.pdf
    """
    SEQ_FIELDS = ["var_attributes", "signatures"]
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._debug = collections.defaultdict(dict)

    def _read(self):
        self._debug['var_attributes']['start'] = self._io.pos()
        self.var_attributes = EfivarSignatureList.EfiVarAttr(self._io, self, self._root)
        self.var_attributes._read()
        self._debug['var_attributes']['end'] = self._io.pos()
        self._debug['signatures']['start'] = self._io.pos()
        self.signatures = []
        i = 0
        while not self._io.is_eof():
            if not 'arr' in self._debug['signatures']:
                self._debug['signatures']['arr'] = []
            self._debug['signatures']['arr'].append({'start': self._io.pos()})
            _t_signatures = EfivarSignatureList.SignatureList(self._io, self, self._root)
            _t_signatures._read()
            self.signatures.append(_t_signatures)
            self._debug['signatures']['arr'][len(self.signatures) - 1]['end'] = self._io.pos()
            i += 1

        self._debug['signatures']['end'] = self._io.pos()

    class SignatureList(KaitaiStruct):
        """
        .. seealso::
           EFI_SIGNATURE_LIST
        """
        SEQ_FIELDS = ["signature_type", "len_signature_list", "len_signature_header", "len_signature", "header", "signatures"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['signature_type']['start'] = self._io.pos()
            self.signature_type = self._io.read_bytes(16)
            self._debug['signature_type']['end'] = self._io.pos()
            self._debug['len_signature_list']['start'] = self._io.pos()
            self.len_signature_list = self._io.read_u4le()
            self._debug['len_signature_list']['end'] = self._io.pos()
            self._debug['len_signature_header']['start'] = self._io.pos()
            self.len_signature_header = self._io.read_u4le()
            self._debug['len_signature_header']['end'] = self._io.pos()
            self._debug['len_signature']['start'] = self._io.pos()
            self.len_signature = self._io.read_u4le()
            self._debug['len_signature']['end'] = self._io.pos()
            self._debug['header']['start'] = self._io.pos()
            self.header = self._io.read_bytes(self.len_signature_header)
            self._debug['header']['end'] = self._io.pos()
            if self.len_signature > 0:
                self._debug['signatures']['start'] = self._io.pos()
                self._raw_signatures = [None] * (((self.len_signature_list - self.len_signature_header) - 28) // self.len_signature)
                self.signatures = [None] * (((self.len_signature_list - self.len_signature_header) - 28) // self.len_signature)
                for i in range(((self.len_signature_list - self.len_signature_header) - 28) // self.len_signature):
                    if not 'arr' in self._debug['signatures']:
                        self._debug['signatures']['arr'] = []
                    self._debug['signatures']['arr'].append({'start': self._io.pos()})
                    self._raw_signatures[i] = self._io.read_bytes(self.len_signature)
                    _io__raw_signatures = KaitaiStream(BytesIO(self._raw_signatures[i]))
                    _t_signatures = EfivarSignatureList.SignatureData(_io__raw_signatures, self, self._root)
                    _t_signatures._read()
                    self.signatures[i] = _t_signatures
                    self._debug['signatures']['arr'][i]['end'] = self._io.pos()

                self._debug['signatures']['end'] = self._io.pos()


        @property
        def is_cert_sha512_x509(self):
            """SHA512 hash of an X.509 certificate's To-Be-Signed contents, and a time of revocation.
            
            .. seealso::
               EFI_CERT_X509_SHA512_GUID
            """
            if hasattr(self, '_m_is_cert_sha512_x509'):
                return self._m_is_cert_sha512_x509 if hasattr(self, '_m_is_cert_sha512_x509') else None

            self._m_is_cert_sha512_x509 = self.signature_type == b"\x63\xBF\x6D\x44\x02\x25\xDA\x4C\xBC\xFA\x24\x65\xD2\xB0\xFE\x9D"
            return self._m_is_cert_sha512_x509 if hasattr(self, '_m_is_cert_sha512_x509') else None

        @property
        def is_cert_sha224(self):
            """SHA-224 hash.
            
            .. seealso::
               EFI_CERT_SHA224_GUID
            """
            if hasattr(self, '_m_is_cert_sha224'):
                return self._m_is_cert_sha224 if hasattr(self, '_m_is_cert_sha224') else None

            self._m_is_cert_sha224 = self.signature_type == b"\x33\x52\x6E\x0B\x5C\xA6\xC9\x44\x94\x07\xD9\xAB\x83\xBF\xC8\xBD"
            return self._m_is_cert_sha224 if hasattr(self, '_m_is_cert_sha224') else None

        @property
        def is_cert_x509(self):
            """X.509 certificate.
            
            .. seealso::
               EFI_CERT_X509_GUID
            """
            if hasattr(self, '_m_is_cert_x509'):
                return self._m_is_cert_x509 if hasattr(self, '_m_is_cert_x509') else None

            self._m_is_cert_x509 = self.signature_type == b"\xA1\x59\xC0\xA5\xE4\x94\xA7\x4A\x87\xB5\xAB\x15\x5C\x2B\xF0\x72"
            return self._m_is_cert_x509 if hasattr(self, '_m_is_cert_x509') else None

        @property
        def is_cert_sha256_x509(self):
            """SHA256 hash of an X.509 certificate's To-Be-Signed contents, and a time of revocation.
            
            .. seealso::
               EFI_CERT_X509_SHA256_GUID
            """
            if hasattr(self, '_m_is_cert_sha256_x509'):
                return self._m_is_cert_sha256_x509 if hasattr(self, '_m_is_cert_sha256_x509') else None

            self._m_is_cert_sha256_x509 = self.signature_type == b"\x92\xA4\xD2\x3B\xC0\x96\x79\x40\xB4\x20\xFC\xF9\x8E\xF1\x03\xED"
            return self._m_is_cert_sha256_x509 if hasattr(self, '_m_is_cert_sha256_x509') else None

        @property
        def is_cert_rsa2048_key(self):
            """RSA-2048 key (only the modulus since the public key exponent is known to be 0x10001).
            
            .. seealso::
               EFI_CERT_RSA2048_GUID
            """
            if hasattr(self, '_m_is_cert_rsa2048_key'):
                return self._m_is_cert_rsa2048_key if hasattr(self, '_m_is_cert_rsa2048_key') else None

            self._m_is_cert_rsa2048_key = self.signature_type == b"\xE8\x66\x57\x3C\x9C\x26\x34\x4E\xAA\x14\xED\x77\x6E\x85\xB3\xB6"
            return self._m_is_cert_rsa2048_key if hasattr(self, '_m_is_cert_rsa2048_key') else None

        @property
        def is_cert_sha512(self):
            """SHA-512 hash.
            
            .. seealso::
               EFI_CERT_SHA512_GUID
            """
            if hasattr(self, '_m_is_cert_sha512'):
                return self._m_is_cert_sha512 if hasattr(self, '_m_is_cert_sha512') else None

            self._m_is_cert_sha512 = self.signature_type == b"\xAE\x0F\x3E\x09\xC4\xA6\x50\x4F\x9F\x1B\xD4\x1E\x2B\x89\xC1\x9A"
            return self._m_is_cert_sha512 if hasattr(self, '_m_is_cert_sha512') else None

        @property
        def is_cert_sha384(self):
            """SHA-384 hash.
            
            .. seealso::
               EFI_CERT_SHA384_GUID
            """
            if hasattr(self, '_m_is_cert_sha384'):
                return self._m_is_cert_sha384 if hasattr(self, '_m_is_cert_sha384') else None

            self._m_is_cert_sha384 = self.signature_type == b"\x07\x53\x3E\xFF\xD0\x9F\xC9\x48\x85\xF1\x8A\xD5\x6C\x70\x1E\x01"
            return self._m_is_cert_sha384 if hasattr(self, '_m_is_cert_sha384') else None

        @property
        def is_cert_sha1(self):
            """SHA-1 hash.
            
            .. seealso::
               EFI_CERT_SHA1_GUID
            """
            if hasattr(self, '_m_is_cert_sha1'):
                return self._m_is_cert_sha1 if hasattr(self, '_m_is_cert_sha1') else None

            self._m_is_cert_sha1 = self.signature_type == b"\x12\xA5\x6C\x82\x10\xCF\xC9\x4A\xB1\x87\xBE\x01\x49\x66\x31\xBD"
            return self._m_is_cert_sha1 if hasattr(self, '_m_is_cert_sha1') else None

        @property
        def is_cert_rsa2048_sha1(self):
            """RSA-2048 signature of a SHA-1 hash.
            
            .. seealso::
               EFI_CERT_RSA2048_SHA1_GUID
            """
            if hasattr(self, '_m_is_cert_rsa2048_sha1'):
                return self._m_is_cert_rsa2048_sha1 if hasattr(self, '_m_is_cert_rsa2048_sha1') else None

            self._m_is_cert_rsa2048_sha1 = self.signature_type == b"\x4F\x44\xF8\x67\x43\x87\xF1\x48\xA3\x28\x1E\xAA\xB8\x73\x60\x80"
            return self._m_is_cert_rsa2048_sha1 if hasattr(self, '_m_is_cert_rsa2048_sha1') else None

        @property
        def is_cert_sha256(self):
            """SHA-256 hash.
            
            .. seealso::
               EFI_CERT_SHA256_GUID
            """
            if hasattr(self, '_m_is_cert_sha256'):
                return self._m_is_cert_sha256 if hasattr(self, '_m_is_cert_sha256') else None

            self._m_is_cert_sha256 = self.signature_type == b"\x26\x16\xC4\xC1\x4C\x50\x92\x40\xAC\xA9\x41\xF9\x36\x93\x43\x28"
            return self._m_is_cert_sha256 if hasattr(self, '_m_is_cert_sha256') else None

        @property
        def is_cert_sha384_x509(self):
            """SHA384 hash of an X.509 certificate's To-Be-Signed contents, and a time of revocation.
            
            .. seealso::
               EFI_CERT_X509_SHA384_GUID
            """
            if hasattr(self, '_m_is_cert_sha384_x509'):
                return self._m_is_cert_sha384_x509 if hasattr(self, '_m_is_cert_sha384_x509') else None

            self._m_is_cert_sha384_x509 = self.signature_type == b"\x6E\x87\x76\x70\xC2\x80\xE6\x4E\xAA\xD2\x28\xB3\x49\xA6\x86\x5B"
            return self._m_is_cert_sha384_x509 if hasattr(self, '_m_is_cert_sha384_x509') else None

        @property
        def is_cert_rsa2048_sha256(self):
            """RSA-2048 signature of a SHA-256 hash.
            
            .. seealso::
               EFI_CERT_RSA2048_SHA256_GUID
            """
            if hasattr(self, '_m_is_cert_rsa2048_sha256'):
                return self._m_is_cert_rsa2048_sha256 if hasattr(self, '_m_is_cert_rsa2048_sha256') else None

            self._m_is_cert_rsa2048_sha256 = self.signature_type == b"\x90\x61\xB3\xE2\x9B\x87\x3D\x4A\xAD\x8D\xF2\xE7\xBB\xA3\x27\x84"
            return self._m_is_cert_rsa2048_sha256 if hasattr(self, '_m_is_cert_rsa2048_sha256') else None

        @property
        def is_cert_der_pkcs7(self):
            """DER-encoded PKCS #7 version 1.5 [RFC2315].
            
            .. seealso::
               EFI_CERT_TYPE_PKCS7_GUID
            """
            if hasattr(self, '_m_is_cert_der_pkcs7'):
                return self._m_is_cert_der_pkcs7 if hasattr(self, '_m_is_cert_der_pkcs7') else None

            self._m_is_cert_der_pkcs7 = self.signature_type == b"\x9D\xD2\xAF\x4A\xDF\x68\xEE\x49\x8A\xA9\x34\x7D\x37\x56\x65\xA7"
            return self._m_is_cert_der_pkcs7 if hasattr(self, '_m_is_cert_der_pkcs7') else None


    class SignatureData(KaitaiStruct):
        """
        .. seealso::
           EFI_SIGNATURE_DATA
        """
        SEQ_FIELDS = ["owner", "data"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['owner']['start'] = self._io.pos()
            self.owner = self._io.read_bytes(16)
            self._debug['owner']['end'] = self._io.pos()
            self._debug['data']['start'] = self._io.pos()
            self.data = self._io.read_bytes_full()
            self._debug['data']['end'] = self._io.pos()


    class EfiVarAttr(KaitaiStruct):
        """Attributes of a UEFI variable."""
        SEQ_FIELDS = ["enhanced_authenticated_access", "append_write", "time_based_authenticated_write_access", "authenticated_write_access", "hardware_error_record", "runtime_access", "bootservice_access", "non_volatile", "reserved1"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['enhanced_authenticated_access']['start'] = self._io.pos()
            self.enhanced_authenticated_access = self._io.read_bits_int_be(1) != 0
            self._debug['enhanced_authenticated_access']['end'] = self._io.pos()
            self._debug['append_write']['start'] = self._io.pos()
            self.append_write = self._io.read_bits_int_be(1) != 0
            self._debug['append_write']['end'] = self._io.pos()
            self._debug['time_based_authenticated_write_access']['start'] = self._io.pos()
            self.time_based_authenticated_write_access = self._io.read_bits_int_be(1) != 0
            self._debug['time_based_authenticated_write_access']['end'] = self._io.pos()
            self._debug['authenticated_write_access']['start'] = self._io.pos()
            self.authenticated_write_access = self._io.read_bits_int_be(1) != 0
            self._debug['authenticated_write_access']['end'] = self._io.pos()
            self._debug['hardware_error_record']['start'] = self._io.pos()
            self.hardware_error_record = self._io.read_bits_int_be(1) != 0
            self._debug['hardware_error_record']['end'] = self._io.pos()
            self._debug['runtime_access']['start'] = self._io.pos()
            self.runtime_access = self._io.read_bits_int_be(1) != 0
            self._debug['runtime_access']['end'] = self._io.pos()
            self._debug['bootservice_access']['start'] = self._io.pos()
            self.bootservice_access = self._io.read_bits_int_be(1) != 0
            self._debug['bootservice_access']['end'] = self._io.pos()
            self._debug['non_volatile']['start'] = self._io.pos()
            self.non_volatile = self._io.read_bits_int_be(1) != 0
            self._debug['non_volatile']['end'] = self._io.pos()
            self._debug['reserved1']['start'] = self._io.pos()
            self.reserved1 = self._io.read_bits_int_be(24)
            self._debug['reserved1']['end'] = self._io.pos()



