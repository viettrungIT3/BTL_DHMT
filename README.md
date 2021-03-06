# BTL_DHMT
### I. Yêu cầu
1. Thành lập nhóm: nhóm gồn 3 thành viên
    - Phạm Tuấn Anh
    - Nguyễn Việt Trung
    - Cao Đắc Thuận

2. Đề tài: `Mô phỏng phòng bếp bằng OpenGL khả lập trình và Visual C++`

### II. Công việc
1. Xác định bài toán

2. Cài đặt chương trình

    2.1 Khởi tạo project - Trung

    2.2 Kỹ thuật mô hình A - Tuấn Anh

    2.2 Kỹ thuật mô hình B - Trung

    2.3 Kỹ thuật mô hình C - Thuận

3. Kết quả đạt được
# Quy trình thực hiện GIT FLOW
![Quy trình git flow]( ./gitflow.png  )

- Tóm tắt quy trình:
    * Clone code về repository ở máy local
    * Cd đến thư mục chứa code
    * Checkout sang một nhánh mới để làm việc (lưu ý: mỗi một tính năng là một nhánh mới)
    * Sau khi thay đổi, code xong ta sẽ add code và tạo commit
    * Checkout sang nhánh develop để pull code mới nhất về
    * Checkout ngược lại sang nhánh ta đang làm việc
    * Đứng ở đây rebase với nhánh develop để đồng bộ code, nếu xảy ra conflict sẽ xử lý ở bước này
    * Push code lên và tạo pull request
	
- Các câu lệnh tương ứng:
    * ` $ git clone`
    * ` $ git checkout -b nameBranch`
    * ` $ git add *`
    * ` $ git commit -m “fix api get all user”`
    * ` $ git checkout develop`
    * ` $ git pull origin develop`
    * ` $ git checkout nameBranch`
    * ` $ git rebase develop (rebase với nhánh develop để đồng nhất code mới nhất)`
    * Nếu xảy conflict thực hiện hai lệnh sau:
        * ` $ git add `
        * ` $ git rebase –continue`
    * ` $ git push origin nameBranch `
    * Tạo pull request trên remote gitlab

- Lưu ý
    * Mỗi pull request chỉ nên tối đa 1 commit, tối đa 3 file chỉnh sửa (trừ initial project)
    * Kết thúc một ngày đều phải commit lên git, kể cả công việc đang làm dở