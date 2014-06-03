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


datafile="two_stage.out_"
numset="0 54"

plot for [i in numset] \
	datafile.i u 1:2 t "Mag_{".i."}" w lp lw 2 pt 1 axis x1y1, \
	 for [j in numset] \
	datafile.j u 1:3 t "Phase_{".j."}" w lp lw 2 pt 1 axis x1y2
#    EOF
