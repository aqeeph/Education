﻿<%@ Page Language="C#" AutoEventWireup="true" CodeFile="Default.aspx.cs" Inherits="_Default" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title></title>
    <style type="text/css">
        .style1
        {
            color: #FF00FF;
        }
        </style>
    <link href="Styles/Styles.css" rel="stylesheet" type="text/css" />
</head>
<body>
    <form id="form1" runat="server">
        <div id="PageWrapper">
            <div id="Header">Header goes here.</div>
            <div id="MenuWrapper">Menu goes here.</div>
            <div id="MainContent">
                <h1 style="padding: 0px; margin: 0px 0px 10px 0px">
                    Hi there visitor! Welcome to my Web Site :-)</h1>
                <p class="Introduction">
                    I&#39;m glad that you are visiting my web site <a href="http://www.mywebsite.com">
                    http://www.mywebsite.com</a>
                </p>
                <p>
                    <strong>Feel</strong> <span class="style1"><strong>free</strong></span> to have a 
                    <a href="Default.aspx">look around</a>.</p>
            </div>
            <div id="Sidebar">Sidebar goes here.</div>
            <div id="Footer">Footer goes here.</div>
        </div>
    </form>
</body>
</html>
