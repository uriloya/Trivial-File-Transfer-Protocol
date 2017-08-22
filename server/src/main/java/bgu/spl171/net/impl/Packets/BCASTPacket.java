package bgu.spl171.net.impl.Packets;

public class BCASTPacket extends Packet
{
	short del_add;
	String filename;
	
	public BCASTPacket(short op, short del_add, String filename) 
	{
		super(op);
		this.del_add = del_add;
		this.filename = filename;
	}
	
	public short getDel_add()
	{
		return del_add;
	}
	
	public String getFileName()
	{
		return filename;
	}
}
