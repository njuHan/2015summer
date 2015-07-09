package analyze;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;

import Type.Udp;

public class throughput_timestamp {
	public static long ui=0;
	public static ArrayList<String> files_pathes=new ArrayList<String>();
	public void list(String a){
		File path=new File(a);
		File[] files=path.listFiles();
		for(int i=0;i<files.length;i++){
			if(files[i].isDirectory()){
				this.list(files[i].getAbsolutePath());
			}
			else{
				if(files[i].getAbsolutePath().contains("_out.txt")){
					this.files_pathes.add(files[i].getAbsolutePath());
				}
			}
		}
	}
	public static void printinline(String fileName, String content) {
		try {
        //打开一个写文件器，构造函数中的第二个参数true表示以追加形式写文件
			FileWriter writer = new FileWriter(fileName, true);
			writer.write(content);
			writer.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	public void readfile(String filename){
		String des_ip="";
		String des_ip_1="";
		int temp_sum=-1;
		String temp_substring="";
		String haha="";
		int temp=-1;
		if(filename.contains("eth")){
//			des_ip="192:168:8:100"; //Xue
			des_ip="192:168:8:100"; //Shang
		}
		else if(filename.contains("w0")){
			//des_ip="192:168:1:126"; //Xue
			des_ip="192:168:1:221"; //Shang
		}
		else if(filename.contains("w4")){
			//des_ip="192:168:1:166"; //Xue
			des_ip="192:168:1:180"; //Shang
		}
		else{ 
			des_ip="192:168:1:221=========";			
		}
		
		System.out.println("handling file:"+filename+" dest IP="+des_ip);
		
		try {
            // read file content from file       
            FileReader reader = new FileReader(filename);
            BufferedReader br = new BufferedReader(reader);   
            String str = null;          
            while((str = br.readLine()) != null) {
//           	ui+=Long.parseLong(str.split("\t")[1]);
//            	if(str.contains("1514")){
//            		this.ui++;
            	if(str.contains(des_ip)){

            	if(temp_sum==-1 && temp_substring.equals("")){/*First line*/
            		temp_sum+=Integer.parseInt(str.split("\t")[1]);
            		temp_sum+=1;
            		temp=str.indexOf(".");
            		temp_substring=str.substring(0,temp+3);
//            		System.out.println(temp_substring);
            	}
            	else{
            		temp=str.indexOf(".");
            		haha=str.substring(0,temp+3);
            		if(temp_substring.equals(haha)){
            			temp_sum+=Integer.parseInt(str.split("\t")[1]);
            		}
            		else{
            			int k=temp_substring.length();
            			this.printinline(filename+"split.txt", temp_substring+"\t"+temp_sum+"\n");
            			temp_sum=Integer.parseInt(str.split("\t")[1]);
            			temp_substring=haha;
            		}
            	}
            	}            	
            	
            }//while           
            br.close();
            reader.close();          
      }
		catch(FileNotFoundException e){
			e.printStackTrace();
		}
	    catch(IOException e){
	        e.printStackTrace();
	    }
	}
	public void process(){
		for(int i=0;i<this.files_pathes.size();i++){
			if(this.files_pathes.get(i).contains("_out.txt")){
				this.readfile(this.files_pathes.get(i));
			}
		}
	}
	public static void main(String[] args){
		throughput_timestamp uu=new throughput_timestamp();
		uu.list("/home/lwz/Documents/5.7/optimal/");
		uu.process();
//		System.out.println(ui);
	}
}
