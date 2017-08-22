package bgu.spl171.net.impl.Packets;

public class DATAPacket extends Packet
{
	short packetsize;
	short block;
	byte[] data;
	
	public DATAPacket(short op, short packetsize, short block, byte[] data) 
	{
		super(op);
		this.packetsize = packetsize;
		this.block = block;
		this.data = data;
	}
	
	public void setBlock(short b)
	{
		block = b;
	}
	
	public short getBlock()
	{
		return block;
	}
	
	public short getSize()
	{
		return packetsize;
	}
	
	public byte[] getData()
	{
		return data;
	}
}
