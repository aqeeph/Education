﻿<%@ Page Language="C#" AutoEventWireup="true" CodeFile="Containers_Panel.aspx.cs" Inherits="Containers" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
    <div>
    
        <asp:CheckBox ID="CheckBox1" runat="server" AutoPostBack="True" 
            oncheckedchanged="CheckBox1_CheckedChanged" Text="Show Panel" />
        <asp:Panel ID="Panel1" runat="server" Visible="False">
            I am visible now!</asp:Panel>
    
    </div>
    </form>
</body>
</html>
