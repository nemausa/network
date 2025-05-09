using System;
using System.Text;
using System.Runtime.InteropServices;

public class CELLRecvStream
{
    //数据缓冲区
    private byte[] _buffer = null;
    //
    private int _nReadPos = 0;
    //

    public CELLRecvStream(IntPtr data, int len)
    {
        //将C++传入的数据转化为C#的字节数组
        _buffer = new byte[len];
        Marshal.Copy(data, _buffer, 0, len);
    }

    private void pop(int n)
    {
        _nReadPos += n;
    }

    private bool canRead(int n)
    {
        return _buffer.Length - _nReadPos >= n;
    }

    public NetCMD ReadNetCmd()
    {
        return (NetCMD)ReadUInt16();
    }

    public sbyte ReadInt8(sbyte n = 0)
    {
        if (canRead(1))
        {
            n = (sbyte)_buffer[_nReadPos];
            pop(1);
        }
        return n;
    }

    public Int16 ReadInt16(Int16 n = 0)
    {
        if(canRead(2))
        {
            n = BitConverter.ToInt16(_buffer, _nReadPos);
            pop(2);
        }
        return n;
    }

    public Int32 ReadInt32(Int32 n = 0)
    {
        if (canRead(4))
        {
            n = BitConverter.ToInt32(_buffer, _nReadPos);
            pop(4);
        }
        return n;
    }

    public Int64 ReadInt64(Int64 n = 0)
    {
        if (canRead(8))
        {
            n = BitConverter.ToInt64(_buffer, _nReadPos);
            pop(8);
        }
        return n;
    }

    public byte ReadUInt8(byte n = 0)
    {
        if (canRead(1))
        {
            n = _buffer[_nReadPos];
            pop(1);
        }
        return n;
    }

    public UInt16 ReadUInt16(UInt16 n = 0)
    {
        if (canRead(2))
        {
            n = BitConverter.ToUInt16(_buffer, _nReadPos);
            pop(2);
        }
        return n;
    }

    public UInt32 ReadUInt32(UInt32 n = 0)
    {
        if (canRead(4))
        {
            n = BitConverter.ToUInt32(_buffer, _nReadPos);
            pop(4);
        }
        return n;
    }

    public UInt64 ReadUInt64(UInt64 n = 0)
    {
        if (canRead(8))
        {
            n = BitConverter.ToUInt64(_buffer, _nReadPos);
            pop(8);
        }
        return n;
    }

    public float ReadFloat(float n = 0.0f)
    {
        if (canRead(4))
        {
            n = BitConverter.ToSingle(_buffer, _nReadPos);
            pop(4);
        }
        return n;
    }

    public double ReadDouble(double n = 0)
    {
        if (canRead(8))
        {
            n = BitConverter.ToDouble(_buffer, _nReadPos);
            pop(8);
        }
        return n;
    }

    public string ReadString()
    {
        string s = string.Empty;
        int len = ReadInt32();
        if (canRead(len) && len > 0)
        {
            s = Encoding.UTF8.GetString(_buffer, _nReadPos, len);
            pop(len);
        }
        return s;
    }

    public Int32[] ReadInt32s()
    {
        int len = ReadInt32();
        Int32[] data = new Int32[len];
        for (int n = 0; n < len; n++)
        {
            data[n] = ReadInt32();
        }
        return data;
    }
}