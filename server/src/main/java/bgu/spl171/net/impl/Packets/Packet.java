package bgu.spl171.net.impl.Packets;

public class Packet
{
	short opcode;
	
	public Packet()
	{
		opcode = -1;
	}
	
	public Packet(short op)
	{
		opcode = op;
	}
	
	public short getOpcode()
	{
		return opcode;
	}
}
