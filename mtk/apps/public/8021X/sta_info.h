#ifndef STA_INFO_H
#define STA_INFO_H

#ifdef ACCOUNTING_SUPPORT
struct sta_info* Ap_get_sta(struct apd_data *apd, u8 *sa, u8 *apidx, u16 ethertype, int sock, int localauth);
#else
struct sta_info* Ap_get_sta(struct apd_data *apd, u8 *sa, u8 *apidx, u16 ethertype, int sock);
#endif

struct sta_info* Ap_get_sta_radius_identifier(struct apd_data *apd, u8 radius_identifier);
void Ap_sta_hash_add(struct apd_data *apd, struct sta_info *sta);
void Ap_free_sta(struct apd_data *apd, struct sta_info *sta);
void Apd_free_stas(struct apd_data *apd);
void Ap_sta_session_timeout(struct apd_data *apd, struct sta_info *sta, u32 session_timeout);
void Ap_sta_no_session_timeout(struct apd_data *apd, struct sta_info *sta);

#endif /* STA_INFO_H */
