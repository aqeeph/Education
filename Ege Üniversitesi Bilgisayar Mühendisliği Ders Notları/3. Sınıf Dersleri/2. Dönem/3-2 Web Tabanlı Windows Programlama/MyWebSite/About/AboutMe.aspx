<%@ Page Title="About Us" Language="C#" MasterPageFile="~/MasterPages/Frontend.master" AutoEventWireup="true" CodeFile="AboutMe.aspx.cs" Inherits="About_Default" %>

<%--Please email to me@mywebsite.com for copyright.--%>

<asp:Content ID="Content1" ContentPlaceHolderID="head" Runat="Server">
</asp:Content>
<asp:Content ID="Content2" ContentPlaceHolderID="cpMainContent" Runat="Server"> 
    This page is about me.<br />
    <br />
    <asp:Button ID="Button1" runat="server" Text="Button" />
    <br />
    <Ege:Banner ID="Banner2" runat="server" 
        DisplayDirection="Horizontal" />
&nbsp;
</asp:Content>

