# barnaul_data_loader
Barnaul spatial data preprocessing and load to NGW tool as plug-in for NGM

To add plug-int to NGM:

add to wxGISCommon.xml 
<lib path="/usr/local/lib/libngm_barnaul_dataloader.so" name="libngm_barnaul_dataloader"/>

add to wxGISCatalog.xml
<Command name="wxGISBarnaulDataloaderCmd"/>
