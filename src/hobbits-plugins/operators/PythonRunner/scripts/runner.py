from user_script import operate_on_bits
import sys
import json
import base64
import math
import binascii
import os

BIT_MASKS = [
    0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01
]

INVERSE_BIT_MASKS = [
    0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe
]

class OperationCancelled(Exception):
    """Raised when the operation is cancelled"""
    pass

class BitContainer:
  def __init__(self):
    self.bytes = bytearray(0)
    self._length = 0

  @property
  def length(self):
      return self._length

  def set_data(self, byte_data, length):
      self.bytes = byte_data
      self._length = length
  
  def set_length(self, length):
      self.bytes = bytearray(math.ceil(length/8))
      self._length = length

  def at(self, i):
      if self.bytes[int(i/8)] & BIT_MASKS[i%8]:
          return True
      else:
          return False
  
  def set_at(self, i, val):
      if val:
          self.bytes[int(i/8)] = self.bytes[int(i/8)] | BIT_MASKS[i%8]
      else:
          self.bytes[int(i/8)] = self.bytes[int(i/8)] & INVERSE_BIT_MASKS[i%8]

class OperatorHandle:
    def __init__(self):
        pass

    def check_cancelled(self):
        if os.path.exists('abort'):
            raise OperationCancelled("Operation was cancelled")

    def set_progress(self, progress):
        with open('progress', 'w') as progress_file:
            json.dump({"progress": progress}, progress_file)

if __name__ == "__main__":
    input_filename = sys.argv[1]
    output_filename = sys.argv[2]

    with open(input_filename, 'r') as input_file:
        input_json = json.load(input_file)
    
    input_container = BitContainer()
    input_container.set_data(bytearray(base64.b64decode(input_json["data"])), input_json["size"])

    output_container = BitContainer()
    operator_handle = OperatorHandle()

    operate_on_bits(input_container, output_container, operator_handle)

    output_json = {
        "data": base64.b64encode(output_container.bytes).decode("utf-8"),
        "size": output_container.length
    }

    with open(output_filename, 'w') as output_file:
        json.dump(output_json, output_file)