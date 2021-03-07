<?xml version="1.0" encoding="utf-8"?>
<!--
This file is part of Escain Documentor tool

Escain Documentor is free software: you can redistribute it and/or modify 
it under ther terms of the GNU General Public License as published by 
the Free Software Foundation, either version 3 of the License, or 
(at your option) any later version.

Escain Documentor is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
GNU General Public License for more details.

You should have received a copy of the GNU General Public License 
along with Escain Documentor. If not, see <https://www.gnu.org/licenses/>.

Author: Adrian Maire escain (at) gmail.com
-->
<xsl:stylesheet version="2.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output 
method="html"
indent="yes"
version="1.1"
doctype-public="html"
media-type="application/xhtml+xml"
encoding="utf-8"/>
<!--
version="1.1"
standalone="no"
media-type="application/xhtml"
exclude-result-prefixes=""
doctype-public="html"
doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"
omit-xml-declaration="yes"-->
<xsl:template match="/document">
<html lang="en-BG">
<head>
	<title><xsl:value-of select="title[1]/text()"/></title>
	<meta charset="UTF-8"/>	
	<link rel="stylesheet" type="text/css" href="qrc:/style.css"/>
</head>
<body>
<article>
	<div class="signature">
	<span class="version"><xsl:value-of select="version[1]/text()"/></span>
	<span class="title"><xsl:value-of select="title[1]/text()"/></span>
	<span class="author"><xsl:value-of select="author[1]/text()"/></span>
	<div class="license">License GFDL: <br/>
Copyright (C)  2013  Adrian Maire.
Permission is granted to copy, distribute and/or modify this document
under the terms of the GNU Free Documentation License, Version 1.3
or any later version published by the Free Software Foundation;
with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts. A copy of the license is located at www.gnu.org/licenses/fdl.txt.</div>
	</div>
	<xsl:apply-templates select='index | glossary'/>
	<xsl:for-each select="*">
		<xsl:if test="name()='header-1'">
			<article class="section-1">
			<h1>8dl912_numberKey1 <xsl:value-of select="@title"/>
				<a><xsl:attribute name="id"><xsl:value-of select="translate(@title, ' ','')"/></xsl:attribute><xsl:comment/></a>			
			</h1>
			<xsl:apply-templates/>
			</article>
		</xsl:if>
		<xsl:if test="name()='page-break'">
			<div class="page-break"><br/><br/><i>-- Continuing in next page --</i><!--comment--></div>
		</xsl:if>
		<!--<xsl:if test="name()='index'">
			<xsl:apply-templates/>
		</xsl:if>-->
	</xsl:for-each>
</article>
</body>
</html>
</xsl:template>

<xsl:template match="p">
	<xsl:if test="normalize-space(.) !=''">
		<span class="text"><!--<span class="texttab"><xsl:comment/></span>--><xsl:apply-templates/></span><br/>
	</xsl:if>
</xsl:template>

<xsl:template match="ul">
	<ul>
	<xsl:for-each select="li">
		<li><xsl:apply-templates/></li>
	</xsl:for-each>
	</ul>
</xsl:template>

<xsl:template match="ol">
	<ol>
	<xsl:for-each select="li">
		<li><xsl:apply-templates/></li>
	</xsl:for-each>
	</ol>
</xsl:template>

<xsl:template match="page-break">
	<div class="page-break"><br/><br/><i>-- Continuing in next page --</i><!--comment--></div>
</xsl:template>

<xsl:template match="header-2">
	<article class="section-2">
	<h2>8dl912_numberKey2 <xsl:value-of select="@title"/>
	<a><xsl:attribute name="id"><xsl:value-of select="translate(@title, ' ','')"/></xsl:attribute><xsl:comment/></a>
	</h2>
	<hr/>
	<xsl:apply-templates/>
	</article>
</xsl:template>

<xsl:template match="header-3">
	<article class="section-3">
	<h3><xsl:value-of select="@title"/>:
	<a><xsl:attribute name="id"><xsl:value-of select="translate(@title, ' ','')"/></xsl:attribute><xsl:comment/></a>
	</h3>
	<xsl:apply-templates/>
	</article>
</xsl:template>

<xsl:template match="header-4">
	<article class="section-4">
	<h4><xsl:value-of select="@title"/>:
	<a><xsl:attribute name="id"><xsl:value-of select="translate(@title, ' ','')"/></xsl:attribute><xsl:comment/></a>
	</h4>
	<xsl:apply-templates/>
	</article>
</xsl:template>

<xsl:template match="code">
	<xsl:if test="@title">
		<div class="code_title">
			<xsl:if test="@lang">
				<span class="code_lang">[<xsl:value-of select="@lang"/>] </span>
			</xsl:if>
			<xsl:value-of select="@title"/>
		</div>
	</xsl:if>
	<pre>
	<xsl:attribute name="class">code <xsl:value-of select="@type"/></xsl:attribute>
	<xsl:apply-templates/>
	</pre>
</xsl:template>

<xsl:template match="l">
	<span class="code_lighted">
	<xsl:apply-templates/>
	</span>
</xsl:template>

<xsl:template match="b">
	<span style="font-weight: bold;">
	<xsl:apply-templates/>
	</span>
</xsl:template>

<xsl:template match="br">
	<br/>
</xsl:template>

<xsl:key name="dictionary" match="/document/dictionary/term" 
	use="translate(@name, 'ABCDEFGHIJKLMNOPQRSTUVWXYZ', 'abcdefghijklmnopqrstuvwxyz')"/>
<xsl:template match="info">
	<span class="info">
	<xsl:attribute name="title"><!--<xsl:value-of select="key('dictionary', 
		translate(., 'ABCDEFGHIJKLMNOPQRSTUVWXYZ', 'abcdefghijklmnopqrstuvwxyz'))"/>-->Value here</xsl:attribute>
	<xsl:apply-templates/>
	</span>
</xsl:template>

<!--<xsl:template match="index">
	<nav class="index">Index:<hr/>
	<xsl:for-each select="//header-1[parent::document]">
		<a class="title1">1
		<xsl:attribute name="href">#<xsl:value-of select="@title"/></xsl:attribute>
		<span><xsl:number/></span><xsl:value-of select="@title"/>
		</a>
		<xsl:for-each select="header-2">
			<a href="#" class="title2">1
			<xsl:attribute name="href">#<xsl:value-of select="@title"/></xsl:attribute>
			<span class="tab"><xsl:number 
			count="header-1"/>.<xsl:number/></span><xsl:value-of select="@title"/></a>
		</xsl:for-each>
	</xsl:for-each>
	</nav>
</xsl:template>-->

<xsl:template match="image">
	<xsl:if test="@title">
		<figure>
		<xsl:attribute name="class">image_cent</xsl:attribute>
		<img>
		<xsl:attribute name="src"><xsl:value-of select="@src"/></xsl:attribute>
		<xsl:attribute name="alt"><xsl:value-of select="@alt"/></xsl:attribute>
		<xsl:attribute name="class">image</xsl:attribute>
		</img>
		<figcaption><xsl:value-of select="@title"/></figcaption>
		</figure>
	</xsl:if>
	<xsl:if test="not(@title)">
		<img>
		<xsl:attribute name="src"><xsl:value-of select="@src"/></xsl:attribute>
		<xsl:attribute name="alt"><xsl:value-of select="@alt"/></xsl:attribute>
		<xsl:attribute name="class">image</xsl:attribute>
		</img>
	</xsl:if>
</xsl:template>

<xsl:template match="glossary">
	<div class="glossary">
		<h1>Glossary:</h1>
		<ul>
		<xsl:for-each select="//dictionary/term">
			<xsl:sort select="@name"/>
			<li class="term"><span class="term_name"><xsl:value-of select="@name"/></span>: <xsl:value-of select="."/></li>
		</xsl:for-each>
		</ul>
		<hr/>
	</div>
</xsl:template>


</xsl:stylesheet>
