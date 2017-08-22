package bgu.spl171.net.impl.TFTPtpc;
import bgu.spl171.net.impl.TFTP.PacketEncoderDecoder;
import bgu.spl171.net.impl.TFTP.TFTPProtocol;
import bgu.spl171.net.srv.Server;

public class TPCMain
{
	public static void main(String[] args)
	{
		int port = Integer.parseInt(args[0]);
		Server.threadPerClient(port,
				()-> new TFTPProtocol(),
				()-> new PacketEncoderDecoder()).serve();
	}
}
