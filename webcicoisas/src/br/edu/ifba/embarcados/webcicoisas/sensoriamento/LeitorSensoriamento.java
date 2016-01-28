package br.edu.ifba.embarcados.webcicoisas.sensoriamento;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.RandomAccessFile;

public class LeitorSensoriamento implements Runnable {
	private static final String ARQUIVO_PIPE = "/home/isaac/ifba/temp/sensoriamentop";
	//acessa o pipe
	private RandomAccessFile pipe = null;
	private boolean continuar = true;
	private static Integer sensores = 0;
	
	@Override
	public void run() {
		continuar = true;
		try {
			pipe = new RandomAccessFile(ARQUIVO_PIPE,"r");
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			//retorna void e para o metodo
			return;
		}
		// TODO Auto-generated method stub
		while(continuar){
			try {
				String s = pipe.readLine();
				if(s!=null && (!s.equals(""))){
					synchronized(sensores){
						sensores = Integer.parseInt(s);
					}
				}
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		
	}
	
	public void parar(){
		continuar = false;
	}
	
	public static Integer getSensores(){
		synchronized(sensores){
			return sensores;
		}
	}
}