package analyze;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;

import Type.Udp;
 
 
public class analyze {
	public static ArrayList<Udp> dest_array=null;
	public static ArrayList<Udp> source_array=null;
	public static ArrayList<String> files_pathes=new ArrayList<String>();
	public static String basic_path="/home/harry/Documents/4.13/";
	public void initial(){
		this.dest_array=new ArrayList<Udp>();
		this.source_array=new ArrayList<Udp>();
	}
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
	public void print_files_infile() throws FileNotFoundException{
		PrintWriter output=new PrintWriter(this.basic_path+"files_list.txt");
		for(int i=0;i<this.files_pathes.size();i++){
			output.print(this.files_pathes.get(i)+"|");
		}
		output.close();
	}
	public void readfile(String filename,int flag){/*flag0---->source    (*^_^*)    flag1---->dest*/
		Udp temp=new Udp();
		double hour;
		double minute;
		double sec;
		double msec;
		try {
            // read file content from file       
            FileReader reader = new FileReader(filename);
            BufferedReader br = new BufferedReader(reader);   
            String str = null;          
            while((str = br.readLine()) != null) {
            	temp=new Udp();
                 if(str.contains("Nanoseconds")){
                	 temp.no=Integer.parseInt(str.split("\t")[2]);
                	 if(str.split("\t")[3].split(" ")[3].contains(":")){
                		 hour=Double.parseDouble(str.split("\t")[3].split(" ")[3].split(":")[0]);
                		 minute=Double.parseDouble(str.split("\t")[3].split(" ")[3].split(":")[1]);
                		 sec=Double.parseDouble(str.split("\t")[3].split(" ")[3].split(":")[2]);
                		 msec=Double.parseDouble(str.split("\t")[3].split(" ")[5]);
                	 }
                	 else{
                		 hour=Double.parseDouble(str.split("\t")[3].split(" ")[4].split(":")[0]);
                		 minute=Double.parseDouble(str.split("\t")[3].split(" ")[4].split(":")[1]);
                		 sec=Double.parseDouble(str.split("\t")[3].split(" ")[4].split(":")[2]);
                		 msec=Double.parseDouble(str.split("\t")[3].split(" ")[6]);
                	 }
                	 temp.timestamp=hour*3600+minute*60+sec+(msec/1000000);
                 }
                 else{
                	 temp.content=str;
                	 if(flag==0){
                		 this.source_array.add(temp);
                	 }
                	 else{
                		 this.dest_array.add(temp);
                	 }
                 }
            }           
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
	public int error_num(String a,String b){
		int num=0;
		if(a.equals(b)){
			return 0;
		}
		else{
			for(int i=0;i<a.length();i++){
				if(a.charAt(i)!=b.charAt(i))
					num++;
			}
		}
		return num;
	}
	public void calculate(){
		int ida;
		int idb;
		int sum=0;
		int error=0;
		double result=0;
		for(int i=0;i<this.source_array.size();i++){
			for(int j=i;j<this.dest_array.size();j++){
				if(this.source_array.get(i).no==this.dest_array.get(j).no){
					sum+=this.source_array.get(i).content.length();
					error+=this.error_num(this.source_array.get(i).content, this.dest_array.get(j).content);
					break;
				}
			}
		}
		result=error/sum;
		System.out.println(result);
	}
	public void analyze(){
		for(int i=0;i<this.files_pathes.size();i++){
			if(this.files_pathes.get(i).contains("client")){
				this.readfile(this.files_pathes.get(i), 0);
				for(int j=0;j<this.files_pathes.size();j++){
					if(this.files_pathes.get(j).contains("server"+this.files_pathes.get(i).split("client")[1])){
						this.readfile(this.files_pathes.get(j), 1);
					}
				}
				this.calculate();
				this.initial();
			}
		}
	}
	public static void main(String[] args){
		analyze aa=new analyze();
		aa.initial();
		aa.list(basic_path);
		aa.analyze();
    }
 
}