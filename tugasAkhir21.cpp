#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <utility> // untuk pasangan data
#include <cstdlib>// untuk clear screen
#include <sys/stat.h> // untuk mengecek keberadaan file

using namespace std;
#define biru "\033[1;36m"
#define merah "\033[1;31m"
#define hijau "\033[1;32m"
#define reset "\033[0m"

void clearScreen() {
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");q
    #endif
}

// untuk menyimpan data menu warteg, termasuk nama, stok, harga modal, harga jual dan kategori menu
struct MenuItem {
    string nama;
    int stok;
    int modal;
    int harga;
    string kategori; 
};

// unruk menyimpan riwayat transaksi penjualan
struct Transaksi {
    string namaMenu;
    int qty;
    int modalTotal;
    int pendapatan;
    int laba;
};

// untuk menyimpan biaya operasional warteg
struct Pengeluaran {
    string ket;
    int nominal;
};

// untuk menyimpan data pelanggan tetap
struct Member {
    string id;
    string nama;
    string nomor;
    int poin;
    bool aktif;
};

// untuk menyimpan informasi diskon menu
struct Diskon {
    string kode;
    string namaMenu;
    int persen;
    string periode;
};
// untuk data voucher belanja
struct Voucher {
    string kode;
    int nominal;
    int minimalBelanja;
    bool digunakan;
};

// class sebagai pengendali utama
class WartegApp {
private:
    vector<MenuItem> menuList; // menyimpan data menu dri file stok.txt
    vector<Transaksi> trxList; // menyimpan riwayat transaksi
    vector<Pengeluaran> pengList; // menyimpan data pengeluaran
    vector<Member> memberList; // menyimpan data member
    vector<Diskon> diskonList; // menyimpan data diskon
    vector<Voucher> voucherList; // menyimpan data voucher 
    vector<pair<string, int>> keranjang; // menyimpan sementara yang sedang dibeli

    // untuk enkripsi data login
    string encrypt(string text) {
        string hasil = "";
        for (char c : text) hasil += (char)(c + 3);
        return hasil;
    }

    //untuk mengecek file
    bool fileExists(string name) {
        struct stat buffer;
        return (stat(name.c_str(), &buffer) == 0);
    }

    // untuk membuat file.txt jika blm ada
    void ensureUserFile() {
        if (!fileExists("user.txt")) {
            ofstream f("user.txt");

            // Password sesuai perbaikan
            const string ADMIN_USER_CT = "dgplq"; 
            const string ADMIN_PASS_CT = "dgplq456"; 
            const string ADMIN_ROLE_CT = "dgplq"; 

            const string KASIR_USER_CT = "ndvlu"; 
            const string KASIR_PASS_CT = "ndvlu456"; 
            const string KASIR_ROLE_CT = "ndvlu"; 
            
            f << ADMIN_USER_CT << " " << ADMIN_PASS_CT << " " << ADMIN_ROLE_CT << "\n";
            f << KASIR_USER_CT << " " << KASIR_PASS_CT << " " << KASIR_ROLE_CT << "\n";
            f.close();
            cout << "\n[INFO] File user.txt tidak ditemukan, file dibuat otomatis.\n";
        }
    }

    // update loadMenu untuk membaca kategori beserta data menu
    void loadMenu() {
        menuList.clear();
        ifstream f("stok.txt");
        if (!f.is_open()) return;
        MenuItem m;
        // Baca data kategori juga
        while (f >> m.nama >> m.stok >> m.modal >> m.harga >> m.kategori) {
            menuList.push_back(m);
        }
    }

    // update saveMenu untuk menyimpan data menu kedalam file.txt
    void saveMenu() {
        ofstream f("stok.txt");
        for (auto &m : menuList) {
            // Simpan kategori ke file
            f << m.nama << " " << m.stok << " " << m.modal << " " << m.harga << " " << m.kategori << "\n";
        }
    }

    // membaca data transaksi dari file
    void loadTransaksi() {
        trxList.clear();
        ifstream f("transaksi.txt");
        if (!f.is_open()) return;
        Transaksi t;
        while (f >> t.namaMenu >> t.qty >> t.modalTotal >> t.pendapatan >> t.laba) {
            trxList.push_back(t);
        }
    }

    // menyimpan data transaksi kedalam file
    void saveTransaksi() {
        ofstream f("transaksi.txt");
        for (auto &t : trxList) {
            f << t.namaMenu << " " << t.qty << " " << t.modalTotal << " "
              << t.pendapatan << " " << t.laba << "\n";
        }
    }

    // untuk membaca data operasional warteg dari file
    void loadPengeluaran() {
        pengList.clear();
        ifstream f("pengeluaran.txt");
        if (!f.is_open()) return;
        Pengeluaran p;
        while (f >> p.ket >> p.nominal) {
            pengList.push_back(p);
        }
    }

    // untuk menyimpan data transaksi operasional warteg ke dalam file
    void savePengeluaran() {
        ofstream f("pengeluaran.txt");
        for (auto &p : pengList) {
            f << p.ket << " " << p.nominal << "\n";
        }
    }

    // untuk mencari data member dari file
    void loadMember() {
        memberList.clear();
        ifstream f("member.txt");
        if (!f.is_open()) return;
        Member m;
        while (f >> m.id >> m.nama >> m.nomor >> m.poin >> m.aktif) {
            memberList.push_back(m);
        }
    }

    // untuk menyimpan data member ke dalam file
    void saveMember() {
        ofstream f("member.txt");
        for (auto &m : memberList) {
            f << m.id << " " << m.nama << " " << m.nomor << " " << m.poin << " " << m.aktif << "\n";
        }
    }

    // untuk membaca data diskon dari dalam file
    void loadDiskon() {
        diskonList.clear();
        ifstream f("diskon.txt");
        if (!f.is_open()) return;
        Diskon d;
        while (f >> d.kode >> d.namaMenu >> d.persen >> d.periode) {
            diskonList.push_back(d);
        }
    }

    // untuk menyimpan data diskon dalam file
    void saveDiskon() {
        ofstream f("diskon.txt");
        for (auto &d : diskonList) {
            f << d.kode << " " << d.namaMenu << " " << d.persen << " " << d.periode << "\n";
        }
    }

    // untuk membaca data voucher dalam file
    void loadVoucher() {
        voucherList.clear();
        ifstream f("voucher.txt");
        if (!f.is_open()) return;
        Voucher v;
        while (f >> v.kode >> v.nominal >> v.minimalBelanja >> v.digunakan) {
            voucherList.push_back(v);
        }
    }

    //untuk menyimpan data voucher ke dalam file
    void saveVoucher() {
        ofstream f("voucher.txt");
        for (auto &v : voucherList) {
            f << v.kode << " " << v.nominal << " " << v.minimalBelanja << " " << v.digunakan << "\n";
        }
    }
    
    // untuk mencari data member berdasarkan id
    Member* cariMember(string id) {
        for (auto &m : memberList) {
            if (m.id == id) return &m;
        }
        return nullptr;
    }

    // untuk menghitung total harga barang di dalam keranjang
    int hitungTotalKeranjang() {
        int total = 0;
        for (auto &item : keranjang) {
            for (auto &m : menuList) {
                if (m.nama == item.first) {
                    total += m.harga * item.second;
                    break;
                }
            }
        }
        return total;
    }


// untuk membaca seluruh data
public:
    WartegApp() {
        loadMenu();
        loadTransaksi();
        loadPengeluaran();
        loadMember();
        loadDiskon();
        loadVoucher();
    }


    // untuk validasi login user
    bool login(string role) {
        ensureUserFile();
        ifstream f("user.txt");
        if (!f.is_open()) {
            cout << "Gagal membuka user.txt\n";
            return false;
        }

        string u_stored_ct, p_stored_ct, r_stored_ct;
        string inputUser, inputPass;

        cout << "Username: ";
        cin >> inputUser;
        cout << "Password: ";
        cin >> inputPass;

        string inputUser_ct = encrypt(inputUser);
        string inputPass_ct = encrypt(inputPass);
        string inputRole_ct = encrypt(role);

        while (f >> u_stored_ct >> p_stored_ct >> r_stored_ct) {
            if (u_stored_ct == inputUser_ct && 
                p_stored_ct == inputPass_ct && 
                r_stored_ct == inputRole_ct) 
            {
                f.close();
                return true;
            }
        }

        f.close();
        cout << "\n[LOGIN GAGAL] Username atau password salah.\n";
        return false;
    }

    // untuk mencari barang
    void kasirPencarianBarang() {
        string cari;
        cout << "Masukkan nama barang yang dicari: ";
        cin >> cari;
        bool ditemukan = false;
        cout << "\nHasil pencarian:\n";
        cout << "Nama\tStok\tHarga\tKategori\n"; 
        cout << "------------------------------------\n";
        for (auto &m : menuList) {
            if (m.nama.find(cari) != string::npos) {
                cout << m.nama << "\t" << m.stok << "\t" << m.harga << "\t" << m.kategori << endl;
                ditemukan = true;
            }
        }
        if (!ditemukan) cout << "Barang tidak ditemukan!\n";
    }

    // untuk menampilkan menu per kategori
    void kasirKategoriProduk() {
        cout << "\n=== KATEGORI PRODUK ===\n";
        cout << "1. Makanan\n";
        cout << "2. Minuman\n";
        cout << "3. Lauk\n";
        cout << "4. Sayur\n";
        cout << "5. Lainnya\n";
        cout << "Pilih kategori (1-5): ";
        int kategori;
        cin >> kategori;

        string filterKategori = "";
        if (kategori == 1) filterKategori = "Makanan";
        else if (kategori == 2) filterKategori = "Minuman";
        else if (kategori == 3) filterKategori = "Lauk";
        else if (kategori == 4) filterKategori = "Sayur";
        else if (kategori == 5) filterKategori = "Lainnya";
        else {
            cout << "Pilihan tidak valid!\n";
            return;
        }

        cout << "\nDaftar menu kategori " << filterKategori << ":\n";
        cout << "Nama\tStok\tHarga\n";
        bool ada = false;
        
        for (auto &m : menuList) {
            if (m.kategori == filterKategori) {
                cout << m.nama << "\t" << m.stok << "\t" << m.harga << endl;
                ada = true;
            }
        }
        
        if (!ada) cout << "(Tidak ada menu di kategori ini)\n";
    }

    // untuk menambahkan barang ke dalam keranjang
    void kasirTambahKeranjang() {
        string nama;
        int qty;
        cout << "Nama barang: ";
        cin >> nama;
        cout << "Quantity: ";
        cin >> qty;

        for (auto &m : menuList) {
            if (m.nama == nama && m.stok >= qty) {
                keranjang.push_back({nama, qty});
                m.stok -= qty;
                saveMenu();
                cout << "Barang ditambahkan ke keranjang!\n";
                return;
            }
        }
        cout << "Barang tidak ditemukan atau stok tidak cukup!\n";
    }

    // untuk membatalkan barang
    void kasirPembatalanBarang() {
        if (keranjang.empty()) {
            cout << "Keranjang kosong!\n";
            return;
        }
        cout << "Daftar keranjang:\n";
        for (int i = 0; i < (int)keranjang.size(); i++) {
            cout << i + 1 << ". " << keranjang[i].first << " x" << keranjang[i].second << endl;
        }
        int index;
        cout << "Pilih nomor yang ingin dibatalkan: ";
        cin >> index;
        if (index > 0 && index <= (int)keranjang.size()) {
            string nama = keranjang[index - 1].first;
            int qty = keranjang[index - 1].second;
            keranjang.erase(keranjang.begin() + index - 1);

            for (auto &m : menuList) {
                if (m.nama == nama) {
                    m.stok += qty;
                    saveMenu();
                    break;
                }
            }
            cout << "Barang dibatalkan dan stok dikembalikan!\n";
        } else {
            cout << "Pilihan tidak valid!\n";
        }
    }

    // untuk mengecek member
    void kasirCekMember() {
        string id;
        cout << "Masukkan ID Member: ";
        cin >> id;
        Member* mem = cariMember(id);
        if (mem) {
            cout << "Nama : " << mem->nama << endl;
            cout << "Poin : " << mem->poin << endl;
            cout << "Status : " << (mem->aktif ? "Aktif" : "Tidak Aktif") << endl;
        } else {
            cout << "Member tidak ditemukan!\n";
        }
    }

    // menampilkan daftar diskon yang tersedia
    void kasirMenuDiskon() {
        cout << "\n=== DAFTAR DISKON ===\n";
        for (auto &d : diskonList) {
            cout << d.kode << " | " << d.namaMenu << " | " << d.persen << "% | " << d.periode << endl;
        }
    }

    // menambahkan member baru
    void kasirTambahMember() {
        Member m;
        cout << "ID Member baru: ";
        cin >> m.id;
        cout << "Nama: ";
        cin >> m.nama;
        cout << "No HP: ";
        cin >> m.nomor;
        m.poin = 0;
        m.aktif = true;
        memberList.push_back(m);
        saveMember();
        cout << "Member berhasil ditambahkan!\n";
    }

    // untuk menukar poin member dengan voucher
    void kasirTebusVoucher() {
        string id;
        cout << "ID Member: ";
        cin >> id;
        Member* mem = cariMember(id);
        if (mem && mem->poin >= 100) {
            mem->poin -= 100;
            saveMember();
            Voucher v;
            v.kode = "V" + to_string(time(nullptr));
            v.nominal = 5000;
            v.minimalBelanja = 20000;
            v.digunakan = false;
            voucherList.push_back(v);
            saveVoucher();
            cout << "Voucher berhasil dibuat dengan kode: " << v.kode << endl;
        } else {
            cout << "Poin tidak cukup atau member tidak ditemukan!\n";
        }
    }

    // untuk mrnhitung total pembayaran kasir
    void kasirTotalPembayaran() {
        if (keranjang.empty()) {
            cout << "Keranjang masih kosong!\n";
            return;
        }

        int total = hitungTotalKeranjang();
        cout << "\n=== STRUK PEMBELIAN ===\n";
        cout << "Item\tQty\tHarga\tSubtotal\n";
        for (auto &item : keranjang) {
            for (auto &m : menuList) {
                if (m.nama == item.first) {
                    int sub = m.harga * item.second;
                    cout << m.nama << "\t" << item.second << "\t" << m.harga << "\t" << sub << endl;
                }
            }
        }
        cout << "Total sebelum diskon: " << total << endl;

        cout << "Apakah member? (y/n): ";
        char c;
        cin >> c;
        if (c == 'y' || c == 'Y') {
            string id;
            cout << "ID Member: ";
            cin >> id;
            Member* mem = cariMember(id);
            if (mem && mem->aktif) {
                int diskonMember = total * 5 / 100;
                total -= diskonMember;
                cout << "Diskon member 5%: " << diskonMember << endl;
            } else {
                cout << "Member tidak ditemukan / tidak aktif.\n";
            }
        }

        cout << "Masukkan kode voucher (jika ada, kosongkan jika tidak): ";
        string kode;
        cin.ignore(); 
        getline(cin, kode);
        if (!kode.empty()) {
            for (auto &v : voucherList) {
                if (v.kode == kode && !v.digunakan && total >= v.minimalBelanja) {
                    total -= v.nominal;
                    v.digunakan = true;
                    saveVoucher();
                    cout << "Voucher digunakan. Potongan: " << v.nominal << endl;
                    break;
                }
            }
        }

        cout << "Total akhir: " << total << endl;
        cout << "Bayar: ";
        int bayar;
        cin >> bayar;
        if (bayar >= total) {
            cout << "Kembalian: " << bayar - total << endl;
            for (auto &item : keranjang) {
                for (auto &m : menuList) {
                    if (m.nama == item.first) {
                        Transaksi t;
                        t.namaMenu = m.nama;
                        t.qty = item.second;
                        t.modalTotal = m.modal * item.second;
                        t.pendapatan = m.harga * item.second;
                        t.laba = t.pendapatan - t.modalTotal;
                        trxList.push_back(t);
                    }
                }
            }
            saveTransaksi();
            keranjang.clear();
            cout << "Transaksi selesai.\n";
        } else {
            cout << "Uang tidak cukup, transaksi dibatalkan.\n";
        }
    }

    // untuk menu kasir
    void kasirMenu() {
        int pilih;
        while (true) {
            // clearScreen(); // Opsional
            cout << "\n===== MENU KASIR =====\n";
            cout << "1. Pencarian Barang\n";
            cout << "2. Kategori Produk\n";
            cout << "3. Tambah ke Keranjang\n";
            cout << "4. Total & Pembayaran\n";
            cout << "5. Pembatalan Barang\n";
            cout << "6. Member (Cek Member)\n";
            cout << "7. Menu Diskon\n";
            cout << "8. Tambah Member\n";
            cout << "9. Tebus Voucher\n";
            cout << "10. Keluar\n";
            cout << "Pilih: ";
            cin >> pilih;

            switch (pilih) {
            case 1: kasirPencarianBarang(); break;
            case 2: kasirKategoriProduk(); break;
            case 3: kasirTambahKeranjang(); break;
            case 4: kasirTotalPembayaran(); break;
            case 5: kasirPembatalanBarang(); break;
            case 6: kasirCekMember(); break;
            case 7: kasirMenuDiskon(); break;
            case 8: kasirTambahMember(); break;
            case 9: kasirTebusVoucher(); break;
            case 10:
                cout << "Logout kasir...\n";
                return;
            default:
                cout << "Pilihan tidak valid!\n";
            }
        }
    }

    // untuk tambah menu
    void adminTambahMenu() {
        MenuItem m;
        cout << "Nama menu: ";
        cin >> m.nama;
        cout << "Stok awal: ";
        cin >> m.stok;
        cout << "Harga modal: ";
        cin >> m.modal;
        cout << "Harga jual: ";
        cin >> m.harga;
        
        cout << "Pilih Kategori:\n";
        cout << "1. Makanan\n2. Minuman\n3. Lauk\n4. Sayur\n5. Lainnya\n";
        cout << "Pilih: ";
        int kat;
        cin >> kat;
        
        if (kat == 1) m.kategori = "Makanan";
        else if (kat == 2) m.kategori = "Minuman";
        else if (kat == 3) m.kategori = "Lauk";
        else if (kat == 4) m.kategori = "Sayur";
        else m.kategori = "Lainnya";

        if (m.stok < 0 || m.modal < 0 || m.harga < 0) {
            cout << "Input tidak valid!\n";
            return;
        }

        menuList.push_back(m);
        saveMenu();

        if (m.stok > 0 && m.modal > 0) {
            Pengeluaran p;
            p.ket = "Stok_Awal_" + m.nama;
            p.nominal = m.stok * m.modal;
            
            pengList.push_back(p);
            savePengeluaran();
        }
        cout << "Menu berhasil ditambahkan ke kategori " << m.kategori << "!\n";
    }

    // untuk edit data menu
    void adminEditMenu() {
        string nama;
        cout << "Nama menu yang ingin diedit: ";
        cin >> nama;
        for (auto &m : menuList) {
            if (m.nama == nama) {
                cout << "Stok baru: ";
                cin >> m.stok;
                cout << "Harga modal baru: ";
                cin >> m.modal;
                cout << "Harga jual baru: ";
                cin >> m.harga;
                
                cout << "Edit Kategori? (y/n): ";
                char c; cin >> c;
                if(c == 'y' || c == 'Y'){
                    cout << "1. Makanan\n2. Minuman\n3. Lauk\n4. Sayur\n5. Lainnya\nPilih: ";
                    int kat; cin >> kat;
                    if (kat == 1) m.kategori = "Makanan";
                    else if (kat == 2) m.kategori = "Minuman";
                    else if (kat == 3) m.kategori = "Lauk";
                    else if (kat == 4) m.kategori = "Sayur";
                    else m.kategori = "Lainnya";
                }

                saveMenu();
                cout << "Menu berhasil diperbarui!\n";
                return;
            }
        }
        cout << "Menu tidak ditemukan!\n";
    }

    // untuk hapus data menu
    void adminHapusMenu() {
        string nama;
        cout << "Nama menu yang ingin dihapus: ";
        cin >> nama;
        for (auto it = menuList.begin(); it != menuList.end(); ++it) {
            if (it->nama == nama) {
                menuList.erase(it);
                saveMenu();
                cout << "Menu berhasil dihapus!\n";
                return;
            }
        }
        cout << "Menu tidak ditemukan!\n";
    }

    // untuk mencatat pengeluaran operasional
    void adminTambahPengeluaran() {
        Pengeluaran p;
        cout << "Keterangan pengeluaran (tanpa spasi): ";
        cin >> p.ket;
        cout << "Nominal: ";
        cin >> p.nominal;
        pengList.push_back(p);
        savePengeluaran();
        cout << "Pengeluaran berhasil dicatat!\n";
    }

    // untuk menghitung dan menampilkan laporan keuangan
    void adminLaporan() {
        int totalModal = 0, totalPendapatan = 0, totalLabaKotor = 0, totalPengeluaran = 0;
        for (auto &t : trxList) {
            totalModal += t.modalTotal;
            totalPendapatan += t.pendapatan;
            totalLabaKotor += t.laba;
        }
        for (auto &p : pengList) {
            totalPengeluaran += p.nominal;
        }
        cout << "\n======== LAPORAN WARTEG ========\n";
        cout << "Total Modal: " << totalModal << endl;
        cout << "Total Pendapatan: " << totalPendapatan << endl;
        cout << "Laba Kotor: " << totalLabaKotor << endl;
        cout << "Pengeluaran: " << totalPengeluaran << endl;
        cout << "Laba Bersih: " << totalLabaKotor - totalPengeluaran << endl;
    }

    // untuk export laporan ke dalam file
    void adminExportLaporan() {
        ofstream f("laporan.txt");
        int totalModal = 0, totalPendapatan = 0, totalLabaKotor = 0, totalPengeluaran = 0;
        for (auto &t : trxList) {
            totalModal += t.modalTotal;
            totalPendapatan += t.pendapatan;
            totalLabaKotor += t.laba;
        }
        for (auto &p : pengList) {
            totalPengeluaran += p.nominal;
        }
        f << "Total Modal: " << totalModal << "\n";
        f << "Total Pendapatan: " << totalPendapatan << "\n";
        f << "Laba Kotor: " << totalLabaKotor << "\n";
        f << "Pengeluaran: " << totalPengeluaran << "\n";
        f << "Laba Bersih: " << totalLabaKotor - totalPengeluaran << "\n";
        f.close();
        cout << "Laporan berhasil diekspor ke laporan.txt\n";
    }

    // untuk menambahkan user
    void adminTambahUser() {
        string user, pass, role;
        cout << "Username baru: ";
        cin >> user;
        cout << "Password baru: ";
        cin >> pass;
        cout << "Role (admin/kasir): ";
        cin >> role;
        
        ofstream f("user.txt", ios::app);
        f << encrypt(user) << " " << encrypt(pass) << " " << encrypt(role) << "\n";
        f.close();
        cout << "User berhasil ditambahkan!\n";
    }

    // untuuk mengedit data member
    void adminEditMember() {
        string id;
        cout << "ID Member yang ingin diedit: ";
        cin >> id;
        for (auto &m : memberList) {
            if (m.id == id) {
                cout << "Nama baru: ";
                cin >> m.nama;
                cout << "No HP baru: ";
                cin >> m.nomor;
                cout << "Poin baru: ";
                cin >> m.poin;
                cout << "Aktif (1 aktif, 0 nonaktif): ";
                cin >> m.aktif;
                saveMember();
                cout << "Data member diperbarui.\n";
                return;
            }
        }
        cout << "Member tidak ditemukan!\n";
    }

    // untuk membuat data diskon
    void adminBuatDiskon() {
        Diskon d;
        cout << "Kode diskon: ";
        cin >> d.kode;
        cout << "Nama menu yang didiskon: ";
        cin >> d.namaMenu;
        cout << "Persentase diskon: ";
        cin >> d.persen;
        cout << "Periode: ";
        cin >> d.periode;
        diskonList.push_back(d);
        saveDiskon();
        cout << "Diskon berhasil dibuat.\n";
    }

    // untuk membuat voucher
    void adminBuatVoucher() {
        Voucher v;
        cout << "Kode voucher: ";
        cin >> v.kode;
        cout << "Nominal potongan: ";
        cin >> v.nominal;
        cout << "Minimal belanja: ";
        cin >> v.minimalBelanja;
        v.digunakan = false;
        voucherList.push_back(v);
        saveVoucher();
        cout << "Voucher berhasil dibuat.\n";
    }

    // untuk menu admin
    void adminMenu() {
        int pilih;
        while (true) {
            cout << "\n===== MENU ADMIN =====\n";
            cout << "1. Tambah Menu\n";
            cout << "2. Edit Menu\n";
            cout << "3. Hapus Menu\n";
            cout << "4. Tambah Pengeluaran\n";
            cout << "5. Lihat Laporan\n";
            cout << "6. Export Laporan\n";
            cout << "7. Tambah User\n";
            cout << "8. Edit Member\n";
            cout << "9. Buat Diskon\n";
            cout << "10. Buat Kode Voucher\n";
            cout << "11. Keluar\n";
            cout << "Pilih: ";
            cin >> pilih;

            switch (pilih) {
            case 1: adminTambahMenu(); break;
            case 2: adminEditMenu(); break;
            case 3: adminHapusMenu(); break;
            case 4: adminTambahPengeluaran(); break;
            case 5: adminLaporan(); break;
            case 6: adminExportLaporan(); break;
            case 7: adminTambahUser(); break;
            case 8: adminEditMember(); break;
            case 9: adminBuatDiskon(); break;
            case 10: adminBuatVoucher(); break;
            case 11:
                cout << "Logout admin...\n";
                return;
            default:
                cout << "Pilihan tidak valid!\n";
            }
        }
    }
};


int main() {
    WartegApp app; 

    int pilihRole;
    cout << biru << "===== APLIKASI WARTEG =====\n"<< reset;
    cout << "1. Login sebagai Admin\n";
    cout << "2. Login sebagai Kasir\n";
    cout << "3. Keluar\n";
    cout << "Pilih: ";
    
    if (!(cin >> pilihRole)) {
        cout << "Input tidak valid.\n";
        return 1;
    }

    if (pilihRole == 1) {
        if (app.login("admin")) app.adminMenu();
        else cout << "Login admin gagal.\n";
    } else if (pilihRole == 2) {
        if (app.login("kasir")) app.kasirMenu();
        else cout << "Login kasir gagal.\n";
    } else if (pilihRole == 3) {
        cout << "Keluar dari program...\n";
    } else {
        cout << "Pilihan tidak dikenal.\n";
    }

    cout << "Program selesai.\n";
    return 0;
}