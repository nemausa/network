﻿using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;

public class CELLSendStream
{
    //数据缓冲区
    private List<byte> _byteList = null;

    public CELLSendStream(int nSzie = 128)
    {
        _byteList = new List<byte>(nSzie);
    }

    public byte[] Array
    {
        get
        {
            return _byteList.ToArray();
        }
    }

    public void setNetCmd(NetCMD cmd)
    {
        WriteUInt16((UInt16)cmd);
    }

    public void finsh()
    {
        if (_byteList.Count > UInt16.MaxValue)
        {
            //Error
        }
        UInt16 len = (UInt16)_byteList.Count;
        //插入一个UInt16后增加的2字节
        len += 2;
        _byteList.InsertRange(0, BitConverter.GetBytes(len));
    }

    public void Write(byte[] data)
    {
        _byteList.AddRange(data);
    }

    public void WriteInt8(sbyte n)
    {
        _byteList.Add((byte)n);
    }

    public void WriteInt16(Int16 n)
    {
        Write(BitConverter.GetBytes(n));
    }

    public void WriteInt32(Int32 n)
    {
        Write(BitConverter.GetBytes(n));
    }

    public void WriteInt64(Int64 n)
    {
        Write(BitConverter.GetBytes(n));
    }

    public void WriteUInt8(byte n)
    {
        _byteList.Add(n);
    }

    public void WriteUInt16(UInt16 n)
    {
        Write(BitConverter.GetBytes(n));
    }

    public void WriteUInt32(UInt32 n)
    {
        Write(BitConverter.GetBytes(n));
    }

    public void WriteUInt64(UInt64 n)
    {
        Write(BitConverter.GetBytes(n));
    }

    public void WriteFloat(float n)
    {
        Write(BitConverter.GetBytes(n));
    }

    public void WriteDouble(double n)
    {
        Write(BitConverter.GetBytes(n));
    }

    //UTF8
    public void WriteString(string s)
    {
        byte[] buffer = Encoding.UTF8.GetBytes(s);
        WriteUInt32((UInt32)buffer.Length + 1);
        Write(buffer);
        WriteUInt8(0);
    }

    public void WriteBytes(byte[] data)
    {
        WriteUInt32((UInt32)data.Length + 1);
        Write(data);
    }

    public void WriteInt32s(Int32[] data)
    {
        WriteUInt32((UInt32)data.Length);
        for (int n = 0; n < data.Length; n++)
        {
            WriteInt32(data[n]);
        }
    }
}
