<%@ Page Title="Welcome" Language="C#" MasterPageFile="~/MasterPages/Frontend.master" AutoEventWireup="true" CodeFile="Default.aspx.cs" Inherits="_Default" %>

<asp:Content ID="Content1" ContentPlaceHolderID="head" Runat="Server">
</asp:Content>
<asp:Content ID="Content2" ContentPlaceHolderID="cpMainContent" Runat="Server">
    <h1 style="padding: 0px; margin: 0px 0px 10px 0px">
        Hi there visitor! Welcome to my Web Site :-)</h1>
    <p class="Introduction">
        I&#39;m glad that you are visiting my web site <a href="http://www.mywebsite.com">
        http://www.mywebsite.com</a>
    </p>
    <p>
        <strong>Feel</strong> <span class="style1"><strong>free</strong></span> to have 
        a <a href="Default.aspx">look around</a>.
    </p>
    <p>
        You can <a href="Login.aspx">log in</a> here
    </p>
</asp:Content>

