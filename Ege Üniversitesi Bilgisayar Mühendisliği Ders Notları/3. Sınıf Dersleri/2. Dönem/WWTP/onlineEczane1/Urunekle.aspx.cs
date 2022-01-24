using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Data;
using System.Data.SqlClient;

public partial class Urunekle : System.Web.UI.Page 

{
    protected void Page_Load(object sender, EventArgs e)
    {
       
    }
    protected void Button4_Click(object sender, EventArgs e)
    {


        SqlConnection baglan = new SqlConnection("Data Source=.\\SQLEXPRESS;AttachDbFilename=\"C:\\Users\\mücahit\\Documents\\Visual Studio 2010\\WebSites\\onlineEczane\\App_Data\\Database.mdf\";Integrated Security=True;User Instance=True");

        SqlCommand sqlComm = new SqlCommand();
        sqlComm = baglan.CreateCommand();




        sqlComm.CommandText = @"INSERT INTO Urun (Adi,Fiyati,Stok_miktari,detaylar,kategori_id) VALUES ('" + TextBox3.Text + "','" + TextBox4.Text + "','" + TextBox5.Text + "','" + TextBox6.Text + "','" + TextBox7.Text + "')";


        baglan.Open();
        sqlComm.ExecuteNonQuery();
        baglan.Close();

        TextBox3.Text = "";
        TextBox4.Text = "";
        TextBox5.Text = "";
        TextBox6.Text = "";
        TextBox7.Text = "";
        Response.Write("<script language='javascript'>alert('Kayit isleminiz Basarili.');</script>");
        
    }
    protected void Urunsilme_Click(object sender, EventArgs e)
    {
        SqlConnection baglan = new SqlConnection("Data Source=.\\SQLEXPRESS;AttachDbFilename=\"C:\\Users\\mücahit\\Documents\\Visual Studio 2010\\WebSites\\onlineEczane\\App_Data\\Database.mdf\";Integrated Security=True;User Instance=True");

        SqlCommand sqlComm = new SqlCommand();
        sqlComm = baglan.CreateCommand();

        sqlComm.CommandText = @"DELETE from Urun  WHERE Urun_id = '" + TextBox8.Text + "'";

        Response.Write("<script language='javascript'>alert('silme isleminiz Basarili.');</script>");
        
        baglan.Open();
        sqlComm.ExecuteNonQuery();
        baglan.Close();
        TextBox8.Text = "";
    }
}