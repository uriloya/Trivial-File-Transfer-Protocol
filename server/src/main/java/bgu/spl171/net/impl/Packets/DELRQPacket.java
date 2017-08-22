package bgu.spl171.net.impl.Packets;

public class DELRQPacket extends Packet
{
	String filename;
	
	public DELRQPacket(short op, String name) 
	{
		super(op);
		filename = name;
	}
	
	public String getFileName()
	{
		return filename;
	}
}
