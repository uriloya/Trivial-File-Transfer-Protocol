package bgu.spl171.net.impl.Packets;

public class ERRORPacket extends Packet
{
	short error;
	String errmsg;
	
	public ERRORPacket(short op, short error, String errmsg) 
	{
		super(op);
		this.error = error;
		this.errmsg = errmsg;
	}
	
	public short getError()
	{
		return error;
	}
	
	public String getMsg()
	{
		return errmsg;
	}
}
