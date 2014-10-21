
"""
  Copyright (c) 2007 Jan-Klaas Kollhof

  This file is part of jsonrpc.

  jsonrpc is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 2.1 of the License, or
  (at your option) any later version.

  This software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this software; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
"""

from types import *
import re

try:
    _unicode = unicode
except NameError:
    _unicode = str

CharReplacements ={
        '\t': '\\t',
        '\b': '\\b',
        '\f': '\\f',
        '\n': '\\n',
        '\r': '\\r',
        '\\': '\\\\',
        '/': '\\/',
        '"': '\\"'}

EscapeCharToChar = {
        't': '\t',
        'b': '\b',
        'f': '\f',
        'n': '\n',
        'r': '\r',
        '\\': '\\',
        '/': '/',
        '"' : '"'}

StringEscapeRE= re.compile(r'[\x00-\x19\\"/\b\f\n\r\t]')
Digits = ['0', '1', '2','3','4','5','6','7','8','9']


class JSONEncodeException(Exception):
    def __init__(self, obj):
        Exception.__init__(self)
        self.obj = obj

    def __str__(self):
       return "Object not encodeable: %s" % self.obj

       
class JSONDecodeException(Exception):
    def __init__(self, message):
        Exception.__init__(self)
        self.message = message

    def __str__(self):
       return self.message

    
def escapeChar(match):
    c=match.group(0)
    try:
        replacement = CharReplacements[c]
        return replacement
    except KeyError:
        d = ord(c)
        if d < 32:
            return '\\u%04x' % d
        else:
            return c

def dumps(obj):
    return _unicode("".join([part for part in dumpParts (obj)]))

def dumpParts (obj):
    try:
        _str = basestring
        _numberTypes = [int, long, float]
    except NameError:
        _str = str
        _numberTypes = [int, float]

    if isinstance(obj, bytes):
        obj = obj.decode()

    if obj is None:
        yield u'null'
    elif isinstance(obj, bool):
        if obj:
            yield u'true'
        else:
            yield u'false'
    elif isinstance(obj, dict):
        yield u'{'
        isFirst=True
        for (key, value) in obj.items():
            if isFirst:
                isFirst=False
            else:
                yield u","
            yield u'"' + StringEscapeRE.sub(escapeChar, key) +u'":'
            for part in dumpParts (value):
                yield part
        yield u'}'
    elif isinstance(obj, _str):
        yield u'"' + StringEscapeRE.sub(escapeChar, obj) +u'"'

    elif any(isinstance(obj, x) for x in [tuple, list, GeneratorType]):
        yield u'['
        isFirst=True
        for item in obj:
            if isFirst:
                isFirst=False
            else:
                yield u","
            for part in dumpParts (item):
                yield part
        yield u']'
    elif any(isinstance(obj, x) for x in _numberTypes):
        yield _unicode(obj)
    else:
        raise JSONEncodeException(obj)
    

def loads(s):
    stack = []
    chars = iter(s)
    value = None
    currCharIsNext=False

    try:
        while(1):
            skip = False
            if not currCharIsNext:
                c = next(chars)
            while(c in [' ', '\t', '\r','\n']):
                c = next(chars)
            currCharIsNext=False
            if c=='"':
                value = ''
                try:
                    c=next(chars)
                    while c != '"':
                        if c == '\\':
                            c=next(chars)
                            try:
                                value+=EscapeCharToChar[c]
                            except KeyError:
                                if c == 'u':
                                    hexCode = next(chars) + next(chars) + next(chars) + next(chars)
                                    value += unichr(int(hexCode,16))
                                else:
                                    raise JSONDecodeException("Bad Escape Sequence Found")
                        else:
                            value+=c
                        c=next(chars)
                except StopIteration:
                    raise JSONDecodeException("Expected end of String")
            elif c == '{':
                stack.append({})
                skip=True
            elif c =='}':
                value = stack.pop()
            elif c == '[':
                stack.append([])
                skip=True
            elif c == ']':
                value = stack.pop()
            elif c in [',',':']:
                skip=True
            elif c in Digits or c == '-':
                digits=[c]
                c = next(chars)
                numConv = int
                try:
                    while c in Digits:
                        digits.append(c)
                        c = next(chars)
                    if c == ".":
                        numConv=float
                        digits.append(c)
                        c = next(chars)
                        while c in Digits:
                            digits.append(c)
                            c = next(chars)
                        if c.upper() == 'E':
                            digits.append(c)
                            c = next(chars)
                            if c in ['+','-']:
                                digits.append(c)
                                c = next(chars)
                                while c in Digits:
                                    digits.append(c)
                                    c = next(chars)
                            else:
                                raise JSONDecodeException("Expected + or -")
                except StopIteration:
                    pass
                value = numConv("".join(digits))
                currCharIsNext=True

            elif c in ['t','f','n']:
                kw = c+ next(chars) + next(chars) + next(chars)
                if kw == 'null':
                    value = None
                elif kw == 'true':
                    value = True
                elif kw == 'fals' and next(chars) == 'e':
                    value = False
                else:
                    raise JSONDecodeException('Expected Null, False or True')
            else:
                raise JSONDecodeException('Expected []{}," or Number, Null, False or True')

            if not skip:
                if len(stack):
                    try:
                        _str = basestring
                    except NameError:
                        _str = str
                    top = stack[-1]
                    if isinstance(top, list):
                        top.append(value)
                    elif isinstance(top, dict):
                        stack.append(value)
                    elif isinstance(top, _str):
                        key = stack.pop()
                        stack[-1][key] = value
                    else:
                        raise JSONDecodeException("Expected dictionary key, or start of a value")
                else:
                    return value
    except StopIteration:
         raise JSONDecodeException("Unexpected end of JSON source")


