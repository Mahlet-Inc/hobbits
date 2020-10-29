# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
from enum import Enum
import collections


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

class MacOsResourceSnd(KaitaiStruct):
    """Sound resources were introduced in Classic MacOS with the Sound Manager program.
    They can contain sound commands to generate sounds with given frequencies as well as sampled sound data.
    They are mostly found in resource forks, but can occasionally appear standalone or embedded in other files.
    
    .. seealso::
       Source - https://developer.apple.com/library/archive/documentation/mac/pdf/Sound/Sound_Manager.pdf
    """

    class CmdType(Enum):
        null_cmd = 0
        quiet_cmd = 3
        flush_cmd = 4
        re_init_cmd = 5
        wait_cmd = 10
        pause_cmd = 11
        resume_cmd = 12
        call_back_cmd = 13
        sync_cmd = 14
        empty_cmd = 15
        available_cmd = 24
        version_cmd = 25
        total_load_cmd = 26
        load_cmd = 27
        freq_duration_cmd = 40
        rest_cmd = 41
        freq_cmd = 42
        amp_cmd = 43
        timbre_cmd = 44
        get_amp_cmd = 45
        volume_cmd = 46
        get_volume_cmd = 47
        wave_table_cmd = 60
        phase_cmd = 61
        sound_cmd = 80
        buffer_cmd = 81
        rate_cmd = 82
        get_rate_cmd = 85

    class SoundHeaderType(Enum):
        standard = 0
        compressed = 254
        extended = 255

    class DataType(Enum):
        square_wave_synth = 1
        wave_table_synth = 3
        sampled_synth = 5

    class WaveInitOption(Enum):
        channel0 = 4
        channel1 = 5
        channel2 = 6
        channel3 = 7

    class InitOption(Enum):
        chan_left = 2
        chan_right = 3
        no_interp = 4
        no_drop = 8
        mono = 128
        stereo = 192
        mace3 = 768
        mace6 = 1024

    class CompressionTypeEnum(Enum):
        variable_compression = -2
        fixed_compression = -1
        not_compressed = 0
        two_to_one = 1
        eight_to_three = 2
        three_to_one = 3
        six_to_one = 4
    SEQ_FIELDS = ["format", "num_data_formats", "data_formats", "reference_count", "num_sound_commands", "sound_commands"]
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._debug = collections.defaultdict(dict)

    def _read(self):
        self._debug['format']['start'] = self._io.pos()
        self.format = self._io.read_u2be()
        self._debug['format']['end'] = self._io.pos()
        if self.format == 1:
            self._debug['num_data_formats']['start'] = self._io.pos()
            self.num_data_formats = self._io.read_u2be()
            self._debug['num_data_formats']['end'] = self._io.pos()

        if self.format == 1:
            self._debug['data_formats']['start'] = self._io.pos()
            self.data_formats = [None] * (self.num_data_formats)
            for i in range(self.num_data_formats):
                if not 'arr' in self._debug['data_formats']:
                    self._debug['data_formats']['arr'] = []
                self._debug['data_formats']['arr'].append({'start': self._io.pos()})
                _t_data_formats = MacOsResourceSnd.DataFormat(self._io, self, self._root)
                _t_data_formats._read()
                self.data_formats[i] = _t_data_formats
                self._debug['data_formats']['arr'][i]['end'] = self._io.pos()

            self._debug['data_formats']['end'] = self._io.pos()

        if self.format == 2:
            self._debug['reference_count']['start'] = self._io.pos()
            self.reference_count = self._io.read_u2be()
            self._debug['reference_count']['end'] = self._io.pos()

        self._debug['num_sound_commands']['start'] = self._io.pos()
        self.num_sound_commands = self._io.read_u2be()
        self._debug['num_sound_commands']['end'] = self._io.pos()
        self._debug['sound_commands']['start'] = self._io.pos()
        self.sound_commands = [None] * (self.num_sound_commands)
        for i in range(self.num_sound_commands):
            if not 'arr' in self._debug['sound_commands']:
                self._debug['sound_commands']['arr'] = []
            self._debug['sound_commands']['arr'].append({'start': self._io.pos()})
            _t_sound_commands = MacOsResourceSnd.SoundCommand(self._io, self, self._root)
            _t_sound_commands._read()
            self.sound_commands[i] = _t_sound_commands
            self._debug['sound_commands']['arr'][i]['end'] = self._io.pos()

        self._debug['sound_commands']['end'] = self._io.pos()

    class Extended(KaitaiStruct):
        SEQ_FIELDS = ["instrument_chunk_ptr", "aes_recording_ptr"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['instrument_chunk_ptr']['start'] = self._io.pos()
            self.instrument_chunk_ptr = self._io.read_u4be()
            self._debug['instrument_chunk_ptr']['end'] = self._io.pos()
            self._debug['aes_recording_ptr']['start'] = self._io.pos()
            self.aes_recording_ptr = self._io.read_u4be()
            self._debug['aes_recording_ptr']['end'] = self._io.pos()


    class SoundHeader(KaitaiStruct):
        SEQ_FIELDS = ["_unnamed0", "sample_ptr", "num_samples", "num_channels", "sample_rate", "loop_start", "loop_end", "encode", "midi_note", "extended_or_compressed", "sample_area"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            if self.start_ofs < 0:
                self._debug['_unnamed0']['start'] = self._io.pos()
                self._unnamed0 = self._io.read_bytes(0)
                self._debug['_unnamed0']['end'] = self._io.pos()

            self._debug['sample_ptr']['start'] = self._io.pos()
            self.sample_ptr = self._io.read_u4be()
            self._debug['sample_ptr']['end'] = self._io.pos()
            if self.sound_header_type == MacOsResourceSnd.SoundHeaderType.standard:
                self._debug['num_samples']['start'] = self._io.pos()
                self.num_samples = self._io.read_u4be()
                self._debug['num_samples']['end'] = self._io.pos()

            if  ((self.sound_header_type == MacOsResourceSnd.SoundHeaderType.extended) or (self.sound_header_type == MacOsResourceSnd.SoundHeaderType.compressed)) :
                self._debug['num_channels']['start'] = self._io.pos()
                self.num_channels = self._io.read_u4be()
                self._debug['num_channels']['end'] = self._io.pos()

            self._debug['sample_rate']['start'] = self._io.pos()
            self.sample_rate = MacOsResourceSnd.UnsignedFixedPoint(self._io, self, self._root)
            self.sample_rate._read()
            self._debug['sample_rate']['end'] = self._io.pos()
            self._debug['loop_start']['start'] = self._io.pos()
            self.loop_start = self._io.read_u4be()
            self._debug['loop_start']['end'] = self._io.pos()
            self._debug['loop_end']['start'] = self._io.pos()
            self.loop_end = self._io.read_u4be()
            self._debug['loop_end']['end'] = self._io.pos()
            self._debug['encode']['start'] = self._io.pos()
            self.encode = KaitaiStream.resolve_enum(MacOsResourceSnd.SoundHeaderType, self._io.read_u1())
            self._debug['encode']['end'] = self._io.pos()
            self._debug['midi_note']['start'] = self._io.pos()
            self.midi_note = self._io.read_u1()
            self._debug['midi_note']['end'] = self._io.pos()
            if  ((self.sound_header_type == MacOsResourceSnd.SoundHeaderType.extended) or (self.sound_header_type == MacOsResourceSnd.SoundHeaderType.compressed)) :
                self._debug['extended_or_compressed']['start'] = self._io.pos()
                self.extended_or_compressed = MacOsResourceSnd.ExtendedOrCompressed(self._io, self, self._root)
                self.extended_or_compressed._read()
                self._debug['extended_or_compressed']['end'] = self._io.pos()

            if self.sample_ptr == 0:
                self._debug['sample_area']['start'] = self._io.pos()
                self.sample_area = self._io.read_bytes((self.num_samples if self.sound_header_type == MacOsResourceSnd.SoundHeaderType.standard else (((self.extended_or_compressed.num_frames * self.num_channels) * self.extended_or_compressed.bits_per_sample) // 8 if self.sound_header_type == MacOsResourceSnd.SoundHeaderType.extended else (self._io.size() - self._io.pos()))))
                self._debug['sample_area']['end'] = self._io.pos()


        @property
        def start_ofs(self):
            if hasattr(self, '_m_start_ofs'):
                return self._m_start_ofs if hasattr(self, '_m_start_ofs') else None

            self._m_start_ofs = self._io.pos()
            return self._m_start_ofs if hasattr(self, '_m_start_ofs') else None

        @property
        def base_freqeuncy(self):
            """base frequency of sample in Hz
            Calculated with the formula (2 ** ((midi_note - 69) / 12)) * 440
            
            .. seealso::
               Source - https://en.wikipedia.org/wiki/MIDI_tuning_standard
            """
            if hasattr(self, '_m_base_freqeuncy'):
                return self._m_base_freqeuncy if hasattr(self, '_m_base_freqeuncy') else None

            if  ((self.midi_note >= 0) and (self.midi_note < 128)) :
                self._m_base_freqeuncy = self._root.midi_note_to_frequency[self.midi_note]

            return self._m_base_freqeuncy if hasattr(self, '_m_base_freqeuncy') else None

        @property
        def sound_header_type(self):
            if hasattr(self, '_m_sound_header_type'):
                return self._m_sound_header_type if hasattr(self, '_m_sound_header_type') else None

            _pos = self._io.pos()
            self._io.seek((self.start_ofs + 20))
            self._debug['_m_sound_header_type']['start'] = self._io.pos()
            self._m_sound_header_type = KaitaiStream.resolve_enum(MacOsResourceSnd.SoundHeaderType, self._io.read_u1())
            self._debug['_m_sound_header_type']['end'] = self._io.pos()
            self._io.seek(_pos)
            return self._m_sound_header_type if hasattr(self, '_m_sound_header_type') else None


    class UnsignedFixedPoint(KaitaiStruct):
        SEQ_FIELDS = ["integer_part", "fraction_part"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['integer_part']['start'] = self._io.pos()
            self.integer_part = self._io.read_u2be()
            self._debug['integer_part']['end'] = self._io.pos()
            self._debug['fraction_part']['start'] = self._io.pos()
            self.fraction_part = self._io.read_u2be()
            self._debug['fraction_part']['end'] = self._io.pos()

        @property
        def value(self):
            if hasattr(self, '_m_value'):
                return self._m_value if hasattr(self, '_m_value') else None

            self._m_value = (self.integer_part + (self.fraction_part / 65535.0))
            return self._m_value if hasattr(self, '_m_value') else None


    class SoundCommand(KaitaiStruct):
        SEQ_FIELDS = ["is_data_offset", "cmd", "param1", "param2"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['is_data_offset']['start'] = self._io.pos()
            self.is_data_offset = self._io.read_bits_int_be(1) != 0
            self._debug['is_data_offset']['end'] = self._io.pos()
            self._debug['cmd']['start'] = self._io.pos()
            self.cmd = KaitaiStream.resolve_enum(MacOsResourceSnd.CmdType, self._io.read_bits_int_be(15))
            self._debug['cmd']['end'] = self._io.pos()
            self._io.align_to_byte()
            self._debug['param1']['start'] = self._io.pos()
            self.param1 = self._io.read_u2be()
            self._debug['param1']['end'] = self._io.pos()
            self._debug['param2']['start'] = self._io.pos()
            self.param2 = self._io.read_u4be()
            self._debug['param2']['end'] = self._io.pos()

        @property
        def sound_header(self):
            if hasattr(self, '_m_sound_header'):
                return self._m_sound_header if hasattr(self, '_m_sound_header') else None

            if  ((self.is_data_offset) and (self.cmd == MacOsResourceSnd.CmdType.buffer_cmd)) :
                _pos = self._io.pos()
                self._io.seek(self.param2)
                self._debug['_m_sound_header']['start'] = self._io.pos()
                self._m_sound_header = MacOsResourceSnd.SoundHeader(self._io, self, self._root)
                self._m_sound_header._read()
                self._debug['_m_sound_header']['end'] = self._io.pos()
                self._io.seek(_pos)

            return self._m_sound_header if hasattr(self, '_m_sound_header') else None


    class Compressed(KaitaiStruct):
        SEQ_FIELDS = ["format", "reserved", "state_vars_ptr", "left_over_samples_ptr", "compression_id", "packet_size", "synthesizer_id"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['format']['start'] = self._io.pos()
            self.format = (self._io.read_bytes(4)).decode(u"ASCII")
            self._debug['format']['end'] = self._io.pos()
            self._debug['reserved']['start'] = self._io.pos()
            self.reserved = self._io.read_bytes(4)
            self._debug['reserved']['end'] = self._io.pos()
            self._debug['state_vars_ptr']['start'] = self._io.pos()
            self.state_vars_ptr = self._io.read_u4be()
            self._debug['state_vars_ptr']['end'] = self._io.pos()
            self._debug['left_over_samples_ptr']['start'] = self._io.pos()
            self.left_over_samples_ptr = self._io.read_u4be()
            self._debug['left_over_samples_ptr']['end'] = self._io.pos()
            self._debug['compression_id']['start'] = self._io.pos()
            self.compression_id = self._io.read_s2be()
            self._debug['compression_id']['end'] = self._io.pos()
            self._debug['packet_size']['start'] = self._io.pos()
            self.packet_size = self._io.read_u2be()
            self._debug['packet_size']['end'] = self._io.pos()
            self._debug['synthesizer_id']['start'] = self._io.pos()
            self.synthesizer_id = self._io.read_u2be()
            self._debug['synthesizer_id']['end'] = self._io.pos()

        @property
        def compression_type(self):
            if hasattr(self, '_m_compression_type'):
                return self._m_compression_type if hasattr(self, '_m_compression_type') else None

            self._m_compression_type = KaitaiStream.resolve_enum(MacOsResourceSnd.CompressionTypeEnum, self.compression_id)
            return self._m_compression_type if hasattr(self, '_m_compression_type') else None


    class ExtendedOrCompressed(KaitaiStruct):
        SEQ_FIELDS = ["num_frames", "aiff_sample_rate", "marker_chunk", "extended", "compressed", "bits_per_sample", "reserved"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['num_frames']['start'] = self._io.pos()
            self.num_frames = self._io.read_u4be()
            self._debug['num_frames']['end'] = self._io.pos()
            self._debug['aiff_sample_rate']['start'] = self._io.pos()
            self.aiff_sample_rate = self._io.read_bytes(10)
            self._debug['aiff_sample_rate']['end'] = self._io.pos()
            self._debug['marker_chunk']['start'] = self._io.pos()
            self.marker_chunk = self._io.read_u4be()
            self._debug['marker_chunk']['end'] = self._io.pos()
            if self._parent.sound_header_type == MacOsResourceSnd.SoundHeaderType.extended:
                self._debug['extended']['start'] = self._io.pos()
                self.extended = MacOsResourceSnd.Extended(self._io, self, self._root)
                self.extended._read()
                self._debug['extended']['end'] = self._io.pos()

            if self._parent.sound_header_type == MacOsResourceSnd.SoundHeaderType.compressed:
                self._debug['compressed']['start'] = self._io.pos()
                self.compressed = MacOsResourceSnd.Compressed(self._io, self, self._root)
                self.compressed._read()
                self._debug['compressed']['end'] = self._io.pos()

            self._debug['bits_per_sample']['start'] = self._io.pos()
            self.bits_per_sample = self._io.read_u2be()
            self._debug['bits_per_sample']['end'] = self._io.pos()
            if self._parent.sound_header_type == MacOsResourceSnd.SoundHeaderType.extended:
                self._debug['reserved']['start'] = self._io.pos()
                self.reserved = self._io.read_bytes(14)
                self._debug['reserved']['end'] = self._io.pos()



    class DataFormat(KaitaiStruct):
        SEQ_FIELDS = ["id", "options"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['id']['start'] = self._io.pos()
            self.id = KaitaiStream.resolve_enum(MacOsResourceSnd.DataType, self._io.read_u2be())
            self._debug['id']['end'] = self._io.pos()
            self._debug['options']['start'] = self._io.pos()
            self.options = self._io.read_u4be()
            self._debug['options']['end'] = self._io.pos()

        @property
        def init_pan_mask(self):
            """mask for right/left pan values."""
            if hasattr(self, '_m_init_pan_mask'):
                return self._m_init_pan_mask if hasattr(self, '_m_init_pan_mask') else None

            self._m_init_pan_mask = 3
            return self._m_init_pan_mask if hasattr(self, '_m_init_pan_mask') else None

        @property
        def wave_init_channel_mask(self):
            """wave table only, Sound Manager 2.0 and earlier."""
            if hasattr(self, '_m_wave_init_channel_mask'):
                return self._m_wave_init_channel_mask if hasattr(self, '_m_wave_init_channel_mask') else None

            self._m_wave_init_channel_mask = 7
            return self._m_wave_init_channel_mask if hasattr(self, '_m_wave_init_channel_mask') else None

        @property
        def init_stereo_mask(self):
            """mask for mono/stereo values."""
            if hasattr(self, '_m_init_stereo_mask'):
                return self._m_init_stereo_mask if hasattr(self, '_m_init_stereo_mask') else None

            self._m_init_stereo_mask = 192
            return self._m_init_stereo_mask if hasattr(self, '_m_init_stereo_mask') else None

        @property
        def wave_init(self):
            if hasattr(self, '_m_wave_init'):
                return self._m_wave_init if hasattr(self, '_m_wave_init') else None

            if self.id == MacOsResourceSnd.DataType.wave_table_synth:
                self._m_wave_init = KaitaiStream.resolve_enum(MacOsResourceSnd.WaveInitOption, (self.options & self.wave_init_channel_mask))

            return self._m_wave_init if hasattr(self, '_m_wave_init') else None

        @property
        def pan_init(self):
            if hasattr(self, '_m_pan_init'):
                return self._m_pan_init if hasattr(self, '_m_pan_init') else None

            self._m_pan_init = KaitaiStream.resolve_enum(MacOsResourceSnd.InitOption, (self.options & self.init_pan_mask))
            return self._m_pan_init if hasattr(self, '_m_pan_init') else None

        @property
        def init_comp_mask(self):
            """mask for compression IDs."""
            if hasattr(self, '_m_init_comp_mask'):
                return self._m_init_comp_mask if hasattr(self, '_m_init_comp_mask') else None

            self._m_init_comp_mask = 65280
            return self._m_init_comp_mask if hasattr(self, '_m_init_comp_mask') else None

        @property
        def stereo_init(self):
            if hasattr(self, '_m_stereo_init'):
                return self._m_stereo_init if hasattr(self, '_m_stereo_init') else None

            self._m_stereo_init = KaitaiStream.resolve_enum(MacOsResourceSnd.InitOption, (self.options & self.init_stereo_mask))
            return self._m_stereo_init if hasattr(self, '_m_stereo_init') else None

        @property
        def comp_init(self):
            if hasattr(self, '_m_comp_init'):
                return self._m_comp_init if hasattr(self, '_m_comp_init') else None

            self._m_comp_init = KaitaiStream.resolve_enum(MacOsResourceSnd.InitOption, (self.options & self.init_comp_mask))
            return self._m_comp_init if hasattr(self, '_m_comp_init') else None


    @property
    def midi_note_to_frequency(self):
        """Lookup table to convert a MIDI note into a frequency in Hz
        The lookup table represents the formula (2 ** ((midi_note - 69) / 12)) * 440
        
        .. seealso::
           Source - https://en.wikipedia.org/wiki/MIDI_tuning_standard
        """
        if hasattr(self, '_m_midi_note_to_frequency'):
            return self._m_midi_note_to_frequency if hasattr(self, '_m_midi_note_to_frequency') else None

        self._m_midi_note_to_frequency = [8.18, 8.66, 9.18, 9.72, 10.30, 10.91, 11.56, 12.25, 12.98, 13.75, 14.57, 15.43, 16.35, 17.32, 18.35, 19.45, 20.60, 21.83, 23.12, 24.50, 25.96, 27.50, 29.14, 30.87, 32.70, 34.65, 36.71, 38.89, 41.20, 43.65, 46.25, 49.00, 51.91, 55.00, 58.27, 61.74, 65.41, 69.30, 73.42, 77.78, 82.41, 87.31, 92.50, 98.00, 103.83, 110.00, 116.54, 123.47, 130.81, 138.59, 146.83, 155.56, 164.81, 174.61, 185.00, 196.00, 207.65, 220.00, 233.08, 246.94, 261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392.00, 415.30, 440.00, 466.16, 493.88, 523.25, 554.37, 587.33, 622.25, 659.26, 698.46, 739.99, 783.99, 830.61, 880.00, 932.33, 987.77, 1046.50, 1108.73, 1174.66, 1244.51, 1318.51, 1396.91, 1479.98, 1567.98, 1661.22, 1760.00, 1864.66, 1975.53, 2093.00, 2217.46, 2349.32, 2489.02, 2637.02, 2793.83, 2959.96, 3135.96, 3322.44, 3520.00, 3729.31, 3951.07, 4186.01, 4434.92, 4698.64, 4978.03, 5274.04, 5587.65, 5919.91, 6271.93, 6644.88, 7040.00, 7458.62, 7902.13, 8372.02, 8869.84, 9397.27, 9956.06, 10548.08, 11175.30, 11839.82, 12543.85]
        return self._m_midi_note_to_frequency if hasattr(self, '_m_midi_note_to_frequency') else None


