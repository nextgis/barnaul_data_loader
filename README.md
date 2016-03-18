# barnaul_data_loader
Barnaul spatial data preprocessing and load to NGW tool as plug-in for NGM

To add plug-int to NGM:

add to wxGISCommon.xml 
```xml
<lib path="/usr/local/lib/libngm_barnaul_dataloader.so" name="libngm_barnaul_dataloader"/>
```

add to wxGISCatalog.xml
```xml
<Command name="wxGISBarnaulDataloaderCmd"/>
```

Commercial support
----------
Need to fix a bug or add a feature to Barnaul Data Loader? We provide custom development and support for this software. [Contact us](http://nextgis.ru/en/contact/) to discuss options!

[![http://nextgis.com](http://nextgis.ru/img/nextgis.png)](http://nextgis.com)
