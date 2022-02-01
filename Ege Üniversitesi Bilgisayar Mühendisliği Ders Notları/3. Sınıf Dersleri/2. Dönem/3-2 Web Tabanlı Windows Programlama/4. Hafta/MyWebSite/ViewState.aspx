<%@ Page Language="C#" AutoEventWireup="true" CodeFile="ViewState.aspx.cs" Inherits="ViewState" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
    <div>
    
        <asp:TextBox ID="TextBox1" runat="server" EnableViewState="False">ViewState</asp:TextBox>
&nbsp;<asp:Button ID="backgroundColorButton" runat="server" 
            onclick="backgroundColorButton_Click" Text="Background Color" />
&nbsp;<asp:Button ID="fontColorButton" runat="server" onclick="fontColorButton_Click" 
            Text="Font Color" />
    
    </div>
    </form>
</body>
</html>
