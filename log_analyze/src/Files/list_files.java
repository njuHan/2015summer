package Files;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.PrintWriter;
import java.util.ArrayList;

public class list_files {
	public static ArrayList<String> files_pathes=new ArrayList<String>();
	public static String basic_path="/home/lwz/Documents/5.7/optimal/";
	public void list(String a){
		File path=new File(a);
		File[] files=path.listFiles();
		for(int i=0;i<files.length;i++){
			if(files[i].isDirectory()){
				this.list(files[i].getAbsolutePath());
			}
			else{
				if(files[i].getAbsolutePath().endsWith(".cap")){
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
	public static void main(String[] args) throws FileNotFoundException{
		list_files temp=new list_files();
		temp.list(temp.basic_path);
		System.out.println(temp.files_pathes.size());
		temp.print_files_infile();
	}
}
 