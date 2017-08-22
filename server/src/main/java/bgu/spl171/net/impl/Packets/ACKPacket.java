package bgu.spl171.net.impl.Packets;

public class ACKPacket extends Packet
{
	short block;
	
	public ACKPacket(short op, short block) 
	{
		super(op);
		this.block = block;
	}
	
	public short getBlock()
	{
		return block;
	}
}

