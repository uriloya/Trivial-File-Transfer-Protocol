package bgu.spl171.net.impl.TFTP;

import java.nio.charset.StandardCharsets;
import java.util.Arrays;

import bgu.spl171.net.api.bidi.BidiMessageEncoderDecoder;
import bgu.spl171.net.impl.Packets.ACKPacket;
import bgu.spl171.net.impl.Packets.BCASTPacket;
import bgu.spl171.net.impl.Packets.DATAPacket;
import bgu.spl171.net.impl.Packets.DELRQPacket;
import bgu.spl171.net.impl.Packets.DIRQPacket;
import bgu.spl171.net.impl.Packets.DISCPacket;
import bgu.spl171.net.impl.Packets.ERRORPacket;
import bgu.spl171.net.impl.Packets.LOGRQPacket;
import bgu.spl171.net.impl.Packets.Packet;
import bgu.spl171.net.impl.Packets.RWRQPacket;

public class PacketEncoderDecoder implements BidiMessageEncoderDecoder<Packet>
{
	private byte[] bytes;
	private int index;
	private short op;
	private short tempop;
	
	public PacketEncoderDecoder(){
		bytes = new byte[1 << 10];
		index = 0;
	}
	
	@Override
	public Packet decodeNextByte(byte nextByte)
	{
		bytes[index] = nextByte;
		
		if(index == 1)
		{
			op = bytesToShort(bytes);
			if(op == 6)
			{
				index = 0;
				tempop = op;
				op = 0;
				return new DIRQPacket(tempop);
			}	
			else if(op == 10)
			{
				index = 0;
				tempop = op;
				op = 0;
				return new DISCPacket(tempop);
			}
		}
		else if(index == 3)
		{
			if(op == 4)
			{
				byte[] b = {bytes[2],bytes[3]};
				short bl = bytesToShort(b);
				index = 0;
				tempop = op;
				op = 0;
				return new ACKPacket(tempop, bl);
			}
		}
		if(index > 1 && nextByte == '\0')
		{
			if(op == 1 || op == 2)
			{
				int temp = index;
				index = 0;
				tempop = op;
				op = 0;
				return new RWRQPacket(tempop, new String(bytes, 2, temp-1, StandardCharsets.UTF_8));
			}
			else if(op == 7)
			{
				int temp = index;
				index = 0;
				tempop = op;
				op = 0;
				return new LOGRQPacket(tempop, new String(bytes, 2, temp-1, StandardCharsets.UTF_8));
			}
			else if(op == 8)
			{
				int temp = index;
				index = 0;
				tempop = op;
				op = 0;
				return new DELRQPacket(tempop, new String(bytes, 2, temp-1, StandardCharsets.UTF_8));
			}
			else if(op == 5 && index > 3)
			{
				byte[] b = {bytes[2],bytes[3]};
				short er = bytesToShort(b);
				int temp = index;
				index = 0;
				tempop = op;
				op = 0;
				return new ERRORPacket(tempop, er, new String(bytes, 4, temp, StandardCharsets.UTF_8)); 
			}
		}
		if(op == 3 && index > 5)
		{
			byte[] b1 = {bytes[2],bytes[3]};
			short sz = bytesToShort(b1); 
			byte[] b2 = {bytes[4],bytes[5]};
			short blo = bytesToShort(b2);
			if(index == sz + 5)
			{
				byte[] b3 = Arrays.copyOfRange(bytes, 6, index+1);
				index = 0;
				tempop = op;
				op = 0;
				return new DATAPacket(tempop, sz, blo, b3);
			}
		}
		
		index++;
		return null;
	}

	@Override
	public byte[] encode(Packet message) 
	{
		if(message.getOpcode()==3)
		{
			byte[] opcode = shortToBytes(message.getOpcode());
			byte[] packetsize = shortToBytes(((DATAPacket)message).getSize());
			byte[] block = shortToBytes(((DATAPacket)message).getBlock());
			byte[] data = ((DATAPacket)message).getData();
			byte[] ans = new byte[opcode.length + packetsize.length + block.length + data.length];
			ans[0]=opcode[0];
			ans[1]=opcode[1];
			ans[2]=packetsize[0];
			ans[3]=packetsize[1];
			ans[4]=block[0];
			ans[5]=block[1];
			int j=0;
			for(int i = 6; i < ans.length; i++)
			{
				ans[i]=data[j++];
			}
			return ans;
		}
		else if(message.getOpcode()==4)
		{
			byte[] opcode = shortToBytes(message.getOpcode());
			byte[] block = shortToBytes(((ACKPacket)message).getBlock());
			byte[] ans = {opcode[0],opcode[1],block[0],block[1]};
			return ans;
		}
		else if(message.getOpcode()==5)
		{
			byte[] opcode = shortToBytes(message.getOpcode());
			byte[] errcode = shortToBytes(((ERRORPacket)message).getError());
			byte[] msg = ((ERRORPacket)message).getMsg().getBytes();
			byte[] ans = new byte[opcode.length + errcode.length + msg.length];
			ans[0]=opcode[0];
			ans[1]=opcode[1];
			ans[2]=errcode[0];
			ans[3]=errcode[1];
			int j=0;
			for(int i = 4; i < ans.length - 1; i++)
			{
				ans[i]=msg[j++];
			}
			return ans;
		}	
		else if(message.getOpcode()==9)
		{
			byte[] opcode = shortToBytes(message.getOpcode());
			byte deladd = (byte)(((BCASTPacket)message).getDel_add() & 0xFF);
			byte[] filename = ((BCASTPacket)message).getFileName().getBytes();
			byte[] ans = new byte[opcode.length + 1 + filename.length];
			ans[0]=opcode[0];
			ans[1]=opcode[1];
			ans[2]=deladd;
			int j=0;
			for(int i = 3; i < ans.length; i++)
			{
				ans[i]=filename[j++];
			}
			return ans;
		}	
	
		return null;
	}
	
    public short bytesToShort(byte[] byteArr)
    {
        short result = (short)((byteArr[0] & 0xff) << 8);
        result += (short)(byteArr[1] & 0xff);
        return result;
    }
    
    public byte[] shortToBytes(short num)
    {
        byte[] bytesArr = new byte[2];
        bytesArr[0] = (byte)((num >> 8) & 0xFF);
        bytesArr[1] = (byte)(num & 0xFF);
        return bytesArr;
    }
}