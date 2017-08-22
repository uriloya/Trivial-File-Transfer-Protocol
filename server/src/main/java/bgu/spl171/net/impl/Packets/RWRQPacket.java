package bgu.spl171.net.impl.Packets;

public class RWRQPacket extends Packet
{
	String filename;
	
	public RWRQPacket(short op, String name) 
	{
		super(op);
		filename = name;
	}
	
	public String getFileName()
	{
		return filename;
	}
}