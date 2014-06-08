#!/usr/bin/gnuplot
#
#    
#    	G N U P L O T
#    	Version 4.6 patchlevel 0    last modified 2012-03-04 
#    	Build System: Linux i686
#    
#    	Copyright (C) 1986-1993, 1998, 2004, 2007-2012
#    	Thomas Williams, Colin Kelley and many others
#    
#    	gnuplot home:     http://www.gnuplot.info
#    	faq, bugs, etc:   type "help FAQ"
#    	immediate help:   type "help"  (plot window: hit 'h')
set terminal postscript enhanced "Helvetica" 15 color
set output "1.eps"
set logscale x
set xlabel "Frequency (Hz)"
set ylabel "Magnitude"
set y2label "Phase"
set y2tics
set ytics nomirror
set grid
set key box left bottom


datafile="/PMOS_CS_low_gain_"
last=".out_0"
numset="original 1st_20dB 1st_40dB 2nd_20dB 2nd_40dB no_gain"

plot for [str1 in numset] \
	str1.datafile.str1.last u 1:2 t "Mag_{".str1."}" w lp lw 2 pt 1 axis x1y1, \
	 for [str2 in numset] \
	str2.datafile.str2.last u 1:3 t "Phase_{".str2."}" w lp lw 2 pt 1 axis x1y2
#    EOF
