from distutils.core import setup
import py2exe

packages= [
    'reportlab',
    'reportlab.lib',
    'reportlab.pdfbase',
    'reportlab.pdfgen',
    'reportlab.platypus',
]

setup(
    console=['read6530.py'],      # change to windows=[...]
    options = {
        "py2exe": { "dll_excludes": ["MSVCP90.dll"],
                    "packages": packages
				  }
                  }
  )