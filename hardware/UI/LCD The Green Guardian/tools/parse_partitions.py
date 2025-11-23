from pathlib import Path
p=Path('.pio\\build\\esp32-s3-devkitc-1\\partitions.bin')
if not p.exists():
    print('partitions.bin not found at',p)
    raise SystemExit(1)
b=p.read_bytes()
labels=[b'otadata',b'app0',b'app1',b'nvs',b'spiffs']
for lab in labels:
    idx=b.find(lab)
    if idx!=-1:
        start=max(0,idx-16)
        print(lab.decode(), 'found at', hex(idx), 'near bytes:', b[start:idx+32].hex())
print('\nScanning as 32-byte entries:')
for i in range(0, len(b), 32):
    entry=b[i:i+32]
    if len(entry)<32: break
    if entry[0]==0xAA and entry[1]==0x50:
        type_sub=entry[2:4]
        offset=int.from_bytes(entry[4:8],'little')
        size=int.from_bytes(entry[8:12],'little')
        label=entry[12:28].split(b'\x00',1)[0].decode(errors='ignore')
        print(hex(i), label, 'type_sub', type_sub.hex(), 'offset', hex(offset), 'size', hex(size))
