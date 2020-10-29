# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
from enum import Enum
import collections


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

class PhpSerializedValue(KaitaiStruct):
    """A serialized PHP value, in the format used by PHP's built-in `serialize` and
    `unserialize` functions. This format closely mirrors PHP's data model:
    it supports all of PHP's scalar types (`NULL`, booleans, numbers, strings),
    associative arrays, objects, and recursive data structures using references.
    The only PHP values not supported by this format are *resources*,
    which usually correspond to native file or connection handles and cannot be
    meaningfully serialized.
    
    There is no official documentation for this data format;
    this spec was created based on the PHP source code and the behavior of
    `serialize`/`unserialize`. PHP makes no guarantees about compatibility of
    serialized data between PHP versions, but in practice, the format has
    remained fully backwards-compatible - values serialized by an older
    PHP version can be unserialized on any newer PHP version.
    This spec supports serialized values from PHP 7.3 or any earlier version.
    
    .. seealso::
       Source - https://www.php.net/manual/en/function.serialize.php
    
    
    .. seealso::
       Source - https://www.php.net/manual/en/function.serialize.php#66147
    
    
    .. seealso::
       Source - https://www.php.net/manual/en/function.unserialize.php
    
    
    .. seealso::
       Source - https://github.com/php/php-src/blob/php-7.3.5/ext/standard/var_unserializer.re
    
    
    .. seealso::
       Source - https://github.com/php/php-src/blob/php-7.3.5/ext/standard/var.c#L822
    """

    class ValueType(Enum):
        custom_serialized_object = 67
        null = 78
        object = 79
        variable_reference = 82
        php_6_string = 83
        array = 97
        bool = 98
        float = 100
        int = 105
        php_3_object = 111
        object_reference = 114
        string = 115

    class BoolValue(Enum):
        false = 48
        true = 49
    SEQ_FIELDS = ["type", "contents"]
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._debug = collections.defaultdict(dict)

    def _read(self):
        self._debug['type']['start'] = self._io.pos()
        self.type = KaitaiStream.resolve_enum(PhpSerializedValue.ValueType, self._io.read_u1())
        self._debug['type']['end'] = self._io.pos()
        self._debug['contents']['start'] = self._io.pos()
        _on = self.type
        if _on == PhpSerializedValue.ValueType.custom_serialized_object:
            self.contents = PhpSerializedValue.CustomSerializedObjectContents(self._io, self, self._root)
            self.contents._read()
        elif _on == PhpSerializedValue.ValueType.php_3_object:
            self.contents = PhpSerializedValue.Php3ObjectContents(self._io, self, self._root)
            self.contents._read()
        elif _on == PhpSerializedValue.ValueType.object:
            self.contents = PhpSerializedValue.ObjectContents(self._io, self, self._root)
            self.contents._read()
        elif _on == PhpSerializedValue.ValueType.variable_reference:
            self.contents = PhpSerializedValue.IntContents(self._io, self, self._root)
            self.contents._read()
        elif _on == PhpSerializedValue.ValueType.php_6_string:
            self.contents = PhpSerializedValue.StringContents(self._io, self, self._root)
            self.contents._read()
        elif _on == PhpSerializedValue.ValueType.float:
            self.contents = PhpSerializedValue.FloatContents(self._io, self, self._root)
            self.contents._read()
        elif _on == PhpSerializedValue.ValueType.object_reference:
            self.contents = PhpSerializedValue.IntContents(self._io, self, self._root)
            self.contents._read()
        elif _on == PhpSerializedValue.ValueType.null:
            self.contents = PhpSerializedValue.NullContents(self._io, self, self._root)
            self.contents._read()
        elif _on == PhpSerializedValue.ValueType.bool:
            self.contents = PhpSerializedValue.BoolContents(self._io, self, self._root)
            self.contents._read()
        elif _on == PhpSerializedValue.ValueType.int:
            self.contents = PhpSerializedValue.IntContents(self._io, self, self._root)
            self.contents._read()
        elif _on == PhpSerializedValue.ValueType.array:
            self.contents = PhpSerializedValue.ArrayContents(self._io, self, self._root)
            self.contents._read()
        elif _on == PhpSerializedValue.ValueType.string:
            self.contents = PhpSerializedValue.StringContents(self._io, self, self._root)
            self.contents._read()
        self._debug['contents']['end'] = self._io.pos()

    class CountPrefixedMapping(KaitaiStruct):
        """A mapping (a sequence of key-value pairs) prefixed with its size."""
        SEQ_FIELDS = ["num_entries_dec", "opening_brace", "entries", "closing_brace"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['num_entries_dec']['start'] = self._io.pos()
            self.num_entries_dec = (self._io.read_bytes_term(58, False, True, True)).decode(u"ASCII")
            self._debug['num_entries_dec']['end'] = self._io.pos()
            self._debug['opening_brace']['start'] = self._io.pos()
            self.opening_brace = self._io.read_bytes(1)
            self._debug['opening_brace']['end'] = self._io.pos()
            if not self.opening_brace == b"\x7B":
                raise kaitaistruct.ValidationNotEqualError(b"\x7B", self.opening_brace, self._io, u"/types/count_prefixed_mapping/seq/1")
            self._debug['entries']['start'] = self._io.pos()
            self.entries = [None] * (self.num_entries)
            for i in range(self.num_entries):
                if not 'arr' in self._debug['entries']:
                    self._debug['entries']['arr'] = []
                self._debug['entries']['arr'].append({'start': self._io.pos()})
                _t_entries = PhpSerializedValue.MappingEntry(self._io, self, self._root)
                _t_entries._read()
                self.entries[i] = _t_entries
                self._debug['entries']['arr'][i]['end'] = self._io.pos()

            self._debug['entries']['end'] = self._io.pos()
            self._debug['closing_brace']['start'] = self._io.pos()
            self.closing_brace = self._io.read_bytes(1)
            self._debug['closing_brace']['end'] = self._io.pos()
            if not self.closing_brace == b"\x7D":
                raise kaitaistruct.ValidationNotEqualError(b"\x7D", self.closing_brace, self._io, u"/types/count_prefixed_mapping/seq/3")

        @property
        def num_entries(self):
            """The number of key-value pairs in the mapping, parsed as an integer.
            """
            if hasattr(self, '_m_num_entries'):
                return self._m_num_entries if hasattr(self, '_m_num_entries') else None

            self._m_num_entries = int(self.num_entries_dec)
            return self._m_num_entries if hasattr(self, '_m_num_entries') else None


    class FloatContents(KaitaiStruct):
        """The contents of a floating-point value."""
        SEQ_FIELDS = ["colon", "value_dec"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['colon']['start'] = self._io.pos()
            self.colon = self._io.read_bytes(1)
            self._debug['colon']['end'] = self._io.pos()
            if not self.colon == b"\x3A":
                raise kaitaistruct.ValidationNotEqualError(b"\x3A", self.colon, self._io, u"/types/float_contents/seq/0")
            self._debug['value_dec']['start'] = self._io.pos()
            self.value_dec = (self._io.read_bytes_term(59, False, True, True)).decode(u"ASCII")
            self._debug['value_dec']['end'] = self._io.pos()


    class LengthPrefixedQuotedString(KaitaiStruct):
        """A quoted string prefixed with its length.
        
        Despite the quotes surrounding the string data, it can contain
        arbitrary bytes, which are never escaped in any way.
        This does not cause any ambiguities when parsing - the bounds of
        the string are determined only by the length field, not by the quotes.
        """
        SEQ_FIELDS = ["len_data_dec", "opening_quote", "data", "closing_quote"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['len_data_dec']['start'] = self._io.pos()
            self.len_data_dec = (self._io.read_bytes_term(58, False, True, True)).decode(u"ASCII")
            self._debug['len_data_dec']['end'] = self._io.pos()
            self._debug['opening_quote']['start'] = self._io.pos()
            self.opening_quote = self._io.read_bytes(1)
            self._debug['opening_quote']['end'] = self._io.pos()
            if not self.opening_quote == b"\x22":
                raise kaitaistruct.ValidationNotEqualError(b"\x22", self.opening_quote, self._io, u"/types/length_prefixed_quoted_string/seq/1")
            self._debug['data']['start'] = self._io.pos()
            self.data = self._io.read_bytes(self.len_data)
            self._debug['data']['end'] = self._io.pos()
            self._debug['closing_quote']['start'] = self._io.pos()
            self.closing_quote = self._io.read_bytes(1)
            self._debug['closing_quote']['end'] = self._io.pos()
            if not self.closing_quote == b"\x22":
                raise kaitaistruct.ValidationNotEqualError(b"\x22", self.closing_quote, self._io, u"/types/length_prefixed_quoted_string/seq/3")

        @property
        def len_data(self):
            """The length of the string's contents in bytes, parsed as an integer.
            The quotes are not counted in this size number.
            """
            if hasattr(self, '_m_len_data'):
                return self._m_len_data if hasattr(self, '_m_len_data') else None

            self._m_len_data = int(self.len_data_dec)
            return self._m_len_data if hasattr(self, '_m_len_data') else None


    class ObjectContents(KaitaiStruct):
        """The contents of an object value serialized in the default format.
        Unlike its PHP 3 counterpart, it contains a class name.
        """
        SEQ_FIELDS = ["colon1", "class_name", "colon2", "properties"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['colon1']['start'] = self._io.pos()
            self.colon1 = self._io.read_bytes(1)
            self._debug['colon1']['end'] = self._io.pos()
            if not self.colon1 == b"\x3A":
                raise kaitaistruct.ValidationNotEqualError(b"\x3A", self.colon1, self._io, u"/types/object_contents/seq/0")
            self._debug['class_name']['start'] = self._io.pos()
            self.class_name = PhpSerializedValue.LengthPrefixedQuotedString(self._io, self, self._root)
            self.class_name._read()
            self._debug['class_name']['end'] = self._io.pos()
            self._debug['colon2']['start'] = self._io.pos()
            self.colon2 = self._io.read_bytes(1)
            self._debug['colon2']['end'] = self._io.pos()
            if not self.colon2 == b"\x3A":
                raise kaitaistruct.ValidationNotEqualError(b"\x3A", self.colon2, self._io, u"/types/object_contents/seq/2")
            self._debug['properties']['start'] = self._io.pos()
            self.properties = PhpSerializedValue.CountPrefixedMapping(self._io, self, self._root)
            self.properties._read()
            self._debug['properties']['end'] = self._io.pos()


    class ArrayContents(KaitaiStruct):
        """The contents of an array value."""
        SEQ_FIELDS = ["colon", "elements"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['colon']['start'] = self._io.pos()
            self.colon = self._io.read_bytes(1)
            self._debug['colon']['end'] = self._io.pos()
            if not self.colon == b"\x3A":
                raise kaitaistruct.ValidationNotEqualError(b"\x3A", self.colon, self._io, u"/types/array_contents/seq/0")
            self._debug['elements']['start'] = self._io.pos()
            self.elements = PhpSerializedValue.CountPrefixedMapping(self._io, self, self._root)
            self.elements._read()
            self._debug['elements']['end'] = self._io.pos()


    class CustomSerializedObjectContents(KaitaiStruct):
        """The contents of an object value that implements a custom
        serialized format using `Serializable`.
        """
        SEQ_FIELDS = ["colon1", "class_name", "colon2", "len_data_dec", "opening_brace", "data", "closing_quote"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['colon1']['start'] = self._io.pos()
            self.colon1 = self._io.read_bytes(1)
            self._debug['colon1']['end'] = self._io.pos()
            if not self.colon1 == b"\x3A":
                raise kaitaistruct.ValidationNotEqualError(b"\x3A", self.colon1, self._io, u"/types/custom_serialized_object_contents/seq/0")
            self._debug['class_name']['start'] = self._io.pos()
            self.class_name = PhpSerializedValue.LengthPrefixedQuotedString(self._io, self, self._root)
            self.class_name._read()
            self._debug['class_name']['end'] = self._io.pos()
            self._debug['colon2']['start'] = self._io.pos()
            self.colon2 = self._io.read_bytes(1)
            self._debug['colon2']['end'] = self._io.pos()
            if not self.colon2 == b"\x3A":
                raise kaitaistruct.ValidationNotEqualError(b"\x3A", self.colon2, self._io, u"/types/custom_serialized_object_contents/seq/2")
            self._debug['len_data_dec']['start'] = self._io.pos()
            self.len_data_dec = (self._io.read_bytes_term(58, False, True, True)).decode(u"ASCII")
            self._debug['len_data_dec']['end'] = self._io.pos()
            self._debug['opening_brace']['start'] = self._io.pos()
            self.opening_brace = self._io.read_bytes(1)
            self._debug['opening_brace']['end'] = self._io.pos()
            if not self.opening_brace == b"\x7B":
                raise kaitaistruct.ValidationNotEqualError(b"\x7B", self.opening_brace, self._io, u"/types/custom_serialized_object_contents/seq/4")
            self._debug['data']['start'] = self._io.pos()
            self.data = self._io.read_bytes(self.len_data)
            self._debug['data']['end'] = self._io.pos()
            self._debug['closing_quote']['start'] = self._io.pos()
            self.closing_quote = self._io.read_bytes(1)
            self._debug['closing_quote']['end'] = self._io.pos()
            if not self.closing_quote == b"\x7D":
                raise kaitaistruct.ValidationNotEqualError(b"\x7D", self.closing_quote, self._io, u"/types/custom_serialized_object_contents/seq/6")

        @property
        def len_data(self):
            """The length of the serialized data in bytes, parsed as an integer.
            The braces are not counted in this length number.
            """
            if hasattr(self, '_m_len_data'):
                return self._m_len_data if hasattr(self, '_m_len_data') else None

            self._m_len_data = int(self.len_data_dec)
            return self._m_len_data if hasattr(self, '_m_len_data') else None


    class NullContents(KaitaiStruct):
        """The contents of a null value (`value_type::null`). This structure
        contains no actual data, since there is only a single `NULL` value.
        """
        SEQ_FIELDS = ["semicolon"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['semicolon']['start'] = self._io.pos()
            self.semicolon = self._io.read_bytes(1)
            self._debug['semicolon']['end'] = self._io.pos()
            if not self.semicolon == b"\x3B":
                raise kaitaistruct.ValidationNotEqualError(b"\x3B", self.semicolon, self._io, u"/types/null_contents/seq/0")


    class Php3ObjectContents(KaitaiStruct):
        """The contents of a PHP 3 object value. Unlike its counterpart in PHP 4
        and above, it does not contain a class name.
        """
        SEQ_FIELDS = ["colon", "properties"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['colon']['start'] = self._io.pos()
            self.colon = self._io.read_bytes(1)
            self._debug['colon']['end'] = self._io.pos()
            if not self.colon == b"\x3A":
                raise kaitaistruct.ValidationNotEqualError(b"\x3A", self.colon, self._io, u"/types/php_3_object_contents/seq/0")
            self._debug['properties']['start'] = self._io.pos()
            self.properties = PhpSerializedValue.CountPrefixedMapping(self._io, self, self._root)
            self.properties._read()
            self._debug['properties']['end'] = self._io.pos()


    class BoolContents(KaitaiStruct):
        """The contents of a boolean value (`value_type::bool`)."""
        SEQ_FIELDS = ["colon", "value_dec", "semicolon"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['colon']['start'] = self._io.pos()
            self.colon = self._io.read_bytes(1)
            self._debug['colon']['end'] = self._io.pos()
            if not self.colon == b"\x3A":
                raise kaitaistruct.ValidationNotEqualError(b"\x3A", self.colon, self._io, u"/types/bool_contents/seq/0")
            self._debug['value_dec']['start'] = self._io.pos()
            self.value_dec = KaitaiStream.resolve_enum(PhpSerializedValue.BoolValue, self._io.read_u1())
            self._debug['value_dec']['end'] = self._io.pos()
            self._debug['semicolon']['start'] = self._io.pos()
            self.semicolon = self._io.read_bytes(1)
            self._debug['semicolon']['end'] = self._io.pos()
            if not self.semicolon == b"\x3B":
                raise kaitaistruct.ValidationNotEqualError(b"\x3B", self.semicolon, self._io, u"/types/bool_contents/seq/2")

        @property
        def value(self):
            """The value of the `bool`, parsed as a boolean."""
            if hasattr(self, '_m_value'):
                return self._m_value if hasattr(self, '_m_value') else None

            self._m_value = self.value_dec == PhpSerializedValue.BoolValue.true
            return self._m_value if hasattr(self, '_m_value') else None


    class StringContents(KaitaiStruct):
        """The contents of a string value.
        
        Note: PHP strings can contain arbitrary byte sequences.
        They are not necessarily valid text in any specific encoding.
        """
        SEQ_FIELDS = ["colon", "string", "semicolon"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['colon']['start'] = self._io.pos()
            self.colon = self._io.read_bytes(1)
            self._debug['colon']['end'] = self._io.pos()
            if not self.colon == b"\x3A":
                raise kaitaistruct.ValidationNotEqualError(b"\x3A", self.colon, self._io, u"/types/string_contents/seq/0")
            self._debug['string']['start'] = self._io.pos()
            self.string = PhpSerializedValue.LengthPrefixedQuotedString(self._io, self, self._root)
            self.string._read()
            self._debug['string']['end'] = self._io.pos()
            self._debug['semicolon']['start'] = self._io.pos()
            self.semicolon = self._io.read_bytes(1)
            self._debug['semicolon']['end'] = self._io.pos()
            if not self.semicolon == b"\x3B":
                raise kaitaistruct.ValidationNotEqualError(b"\x3B", self.semicolon, self._io, u"/types/string_contents/seq/2")

        @property
        def value(self):
            """The value of the string, as a byte array."""
            if hasattr(self, '_m_value'):
                return self._m_value if hasattr(self, '_m_value') else None

            self._m_value = self.string.data
            return self._m_value if hasattr(self, '_m_value') else None


    class IntContents(KaitaiStruct):
        """The contents of an integer-like value:
        either an actual integer (`value_type::int`) or a reference
        (`value_type::variable_reference`, `value_type::object_reference`).
        """
        SEQ_FIELDS = ["colon", "value_dec"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['colon']['start'] = self._io.pos()
            self.colon = self._io.read_bytes(1)
            self._debug['colon']['end'] = self._io.pos()
            if not self.colon == b"\x3A":
                raise kaitaistruct.ValidationNotEqualError(b"\x3A", self.colon, self._io, u"/types/int_contents/seq/0")
            self._debug['value_dec']['start'] = self._io.pos()
            self.value_dec = (self._io.read_bytes_term(59, False, True, True)).decode(u"ASCII")
            self._debug['value_dec']['end'] = self._io.pos()

        @property
        def value(self):
            """The value of the `int`, parsed as an integer."""
            if hasattr(self, '_m_value'):
                return self._m_value if hasattr(self, '_m_value') else None

            self._m_value = int(self.value_dec)
            return self._m_value if hasattr(self, '_m_value') else None


    class MappingEntry(KaitaiStruct):
        """A mapping entry consisting of a key and a value."""
        SEQ_FIELDS = ["key", "value"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['key']['start'] = self._io.pos()
            self.key = PhpSerializedValue(self._io)
            self.key._read()
            self._debug['key']['end'] = self._io.pos()
            self._debug['value']['start'] = self._io.pos()
            self.value = PhpSerializedValue(self._io)
            self.value._read()
            self._debug['value']['end'] = self._io.pos()



