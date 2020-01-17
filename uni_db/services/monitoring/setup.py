import setuptools

with open("README.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    name="nica-db-monitor-pklimai",
    version="0.0.1",
    author="Peter Klimai",
    author_email="pklimai@gmail.com",
    description="Script to monitor PostgreSQL database server",
    long_description=long_description,
    long_description_content_type="text/markdown",
    # url="",
    packages=setuptools.find_packages(),
    install_requires=['psycopg2-binary', 'influxdb'],
    scripts=['nica-db-mon.py'],
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: Linux",
    ],
    python_requires='>=3.4',
)
