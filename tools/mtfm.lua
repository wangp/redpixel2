#!/usr/bin/lua -f

-- mtfm.lua: Make the f*cking manual!
-- Initial version: 27-28 May 2001.


-- Remove leading a trailing whitespace from a string.
function strip(str)
    return gsub(str, "^%s*(.-)%s*$", "%1")
end


-- Return true if a string matches a pattern.
function strmatch(s, pat)
    local _, n = gsub(s, pat, "", 1)
    return n ~= 0
end


-- Read a line from a file, return it stripped of whitespace.
function sread()
    local s = read(); return s and strip(s) or nil
end


-- Helper: guess the name of a symbol, given the prototype/declaration.
function guess_symbol_name(str)
    str = gsub(str, "^typedef%s+struct%s+", "")
    str = gsub(str, "^typedef%s+enum%s+", "")
    str = gsub(str, "^typedef%s+", "")
    str = gsub(str, "^extern%s+", "")
    return gsub(str, "^[%w_]*%s+%*?%s*([%w_]+).*", "%1")
end


-- Helper: extract name, prototype and description from a long comment.
function extract_long(initial)
    local desc, proto = "", ""
	    
    -- extract name
    local name = gsub(initial, "^/%*%s*(%S+):.*", "%1")

    -- extract description
    local s = gsub(initial, "^.-:(.*)", "%1")
    while s do
	local ss = strip(s)
	local stop
	if strsub(ss, -2) == "*/" then
	    ss = strsub(ss, 1, -3); stop = 1
	end
	s = gsub(ss, "^%*", "")
	desc = desc..s.."\n"
	if stop then break end
	s = read()
    end

    -- extract prototype
    s = sread()
    while s do
	proto = proto..s.." "
	if strsub(proto, -2) == "; " or strsub(proto, -2) == "{ " then
	    proto = strsub(proto, 1, -3); break
	end
	s = sread()
    end

    return name, proto, desc
end


-- Helper: extract prototype and description, and guess the name of the 
-- symbol from a short comment.
function extract_short(initial)
    local desc, proto = "", ""

    -- extract description
    local s = strip(gsub(initial, "^/%*:(.*)", "%1"))
    while s do
	local stop
	if strsub(s, -2) == "*/" then s = strsub(s, 1, -3); stop = 1 end
	s = gsub(s, "^%*%s*", "")
	desc = desc..s.."\n"
	if stop then break end
	s = sread()
    end

    -- extract prototype
    s = sread()
    while s do
	proto = proto..s.." "
	if strsub(proto, -2) == "; " or strsub(proto, -2) == "{ " then
	    proto = strsub(proto, 1, -3); break
	end
	s = sread()
    end

    -- guess the name
    local name = guess_symbol_name(proto)

    return name, proto, desc
end


-- Process a file for generation.  Return a table of the form
--  {{ name = foo, proto = bar, desc = quux } ...}
function process(filename)
    local t = {}
    readfrom(filename)

    while 1 do
	local s = sread(); if not s then break end

	if strmatch(s, "^/%*%s*%S+:") then
	    -- /* quux:
	    --  *  This is an extractable comment.
	    --  */
	    -- int quux();
	    local name, proto, desc = extract_long(s)
	    tinsert(t, { name = name, proto = proto, desc = desc })
	elseif strmatch(s, "^/%*:.*") then
	    -- /*: This is a shorter extractable comment.  It will try to
	    --  *  guess the name of the symbol by a simple pattern.  */
	    -- int foo;
	    local name, proto, desc = extract_short(s)
	    tinsert(t, { name = name, proto = proto, desc = desc })
	end
    end

    readfrom()
    t.n = nil
    return t
end


-- Generate index and description in HTML for a single file.
function output_html_helper(filename, t)
    function esc(s)
	s = gsub(s, "<", "&lt;")
	s = gsub(s, ">", "&gt;")
	s = gsub(s, "\n\n", "\n<p>")
	s = gsub(s, "See also:", "<p>See also:")
	return s
    end

    function example(s)
	return gsub(s, "(Example:)(.*)$", 
	            function (a1, a2) 
			return a1.."<pre>"..gsub(a2, "<p>", "").."</pre>"
		    end, 1)
    end

    function external(proto)
	local _, n = gsub(proto, "static ", "")
	return n == 0
    end

    local basename = gsub(filename, ".-([^/]+)$", "%1") 
    local fn = gsub(basename, "%W", "_").."_"
    local idx, desc = "", ""

    -- index
    idx = idx.."\n<table cellpadding=10 border=1 width=90%><tr>"..
               "<td bgcolor=#d0d0ff><b>"..basename.."</b></tr></table>\n<ul>"
    for i,v in t do
	local e = external(v.proto)
	idx = idx.."\n<li>"..
	      format("<a name='idx_%s'></a><a href='#sym_%s'>%s%s%s</a>",
	             fn..v.name, fn..v.name,
	             e and "<b>" or "", esc(v.name), e and "</b>" or "")
    end
    idx = idx.."\n</ul>\n<br><br>"

    -- descriptions
    desc = desc..("\n<table cellpadding=10 border=1 width=90%><tr>"..
                  "<td bgcolor=#ffd0d0><b>"..basename.."</b></tr></table>")
    for i,v in t do
	desc = desc.."\n\n<a name='sym_"..fn..v.name.."'></a>"..
	       format("<p><a href='#idx_%s'>%s</a>",
		      fn..v.name, 
		      gsub(v.proto, v.name, "<b>"..esc(v.name).."</b>", 1))
	if not external(v.proto) then
	    desc = desc.."&nbsp;&nbsp;&nbsp;"..
	    	   "<em><font color=red>[internal]</font></em>"
	end
	desc = desc.."\n<blockquote>\n"..example(esc(v.desc))..
		"</blockquote><br><br>"
    end

    return idx, desc
end


-- Output documentation in HTML form.
function output_html(idx, desc)
    print [[<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 3.2//EN">]]
    print "<html>\n<head><title>Code docs</title></head>"
    print "<body bgcolor=#ffffff text=#000000 link=#0000ff vlink=#0000ff>"
    print(idx)
    print(desc)
    print("\n<hr>\n<em>Generated by mtfm.lua on "..date().."</em>")
    print "</body>\n</html>"
end


-- script entry point
if getn(arg) ~= 0 then
    local idx, desc = "", ""
    for i = 1, getn(arg) do
	local fn = arg[i]
	local t = process(fn)
--	sort(t, function (a, b) return a.name < b.name end)
	local i, d = output_html_helper(fn, t)
	idx = idx..i
	desc = desc..d
    end
    output_html(idx, desc)
end


-- mtfm.lua ends here
