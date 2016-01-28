package br.edu.ifba.embarcados.webcicoisas;

import javax.ws.rs.GET;
//webservice RESTful
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.MediaType;

import br.edu.ifba.embarcados.webcicoisas.sensoriamento.LeitorSensoriamento;

//URL: localhost:8080/webcicoisas/v1/servico/sensores

@Path("servico")
public class Servico {
	@GET
	@Path("sensores")
	@Produces(MediaType.TEXT_PLAIN)
	public String getSensores(){
		System.out.println("Sensor lido: "+LeitorSensoriamento.getSensores()+"");
		return LeitorSensoriamento.getSensores()+"";
	}
}
