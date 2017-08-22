package bgu.spl171.net.impl.Packets;

public class LOGRQPacket extends Packet
{

	String username;
	
	public LOGRQPacket(short op, String name) 
	{
		super(op);
		username = name;
	}
	
	public String getUserName ()
	{
		return username;
	}
}
