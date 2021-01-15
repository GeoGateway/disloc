# GeoGateway Disloc Service

complile disloc.c:
	gcc disloc.c -o disloc -lm   
	
## Testing Environment:
* [Anaconda Python 3.8](https://docs.conda.io/en/latest/miniconda.html)  

## Command line mode:

**disloc_exec.py**    

   required parameters:   
   * -i,--input : disloc input file  
   
   optional:    
   * -o, --output : output file name prefix
   * -el, --elevation : elevation in degrees, default: 60
   * -az, --azimuth : azimuth in degrees, default: 0
   * -rf, --radarfrequency : radarfrequency: in GHz, default: 1.26
   * -wd, --workdir : working directory

output:   
<pre>
   * input.txt                  input file
   * output.csv			disloc output
   * output.csv.insar.kml	Synthetic Interferograms (kml)
   * output.csv.insar.kmz	Synthetic Interferograms (kmz)
   * output.csv.insar.png	Synthetic Interferograms (plot)   
 </pre>
