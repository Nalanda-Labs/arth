<script context="module">
</script>

<script>
    import { goto, stores } from "@sapper/app";
    import { onMount } from "svelte";
    import * as api from "api.js";
    import jwt_decode from "jwt-decode";
    import Swal from "sweetalert2";
    import { mdiScrewLag } from "@mdi/js";

    export let username;
    export let id;
    let name = "";
    let title = "";
    let designation = "";
    let location = "";
    let image_url = "";
    let image_alt = "";
    let response = {};

    let { session } = stores();

    onMount(async () => {
        response = await api.get(
            `users/${id}/${username}`,
            localStorage.getItem("jwt")
        );

        if (!response.error) {
            name = response.name;
            title = response.title;
            designation = response.designation;
            location = response.location;
            username = response.username;

            if (response.image_url !== "") {
                image_url = response.image_url;
                image_alt = name;
            } else {
                image_alt = "No profile photo";
            }
            if (title === "" && $session.user === response.username) {
                title = "Click to edit your title";
            }
            if (name === "" && $session.user === response.username) {
                name = "Click to edit your name";
            }
            if (designation === "" && $session.user === response.username) {
                designation = "Click to edit your designation";
            }
            if (location === "" && $session.user === response.username) {
                location = "Click to edit your location";
            }
        }

        let username_elem = document.getElementById("username");

        username_elem.addEventListener(
            "blur",
            async function () {
                if (username != username_elem.innerHTML) {
                    let username1 = username_elem.innerHTML;
                    response = await api.post(
                        `profile/${id}/username/${username1}/`,
                        username1,
                        localStorage.getItem("jwt")
                    );
                    if (response.error) {
                        Swal.fire(response.error);
                    } else if (response.jwt) {
                        Swal.fire("You will be logged out for username change");
                        await api.post(`auth/logout`);
		                $session.user = null;
                        localStorage.removeItem("jwt");
                        goto("/login");
                    }
                }
            },
            false
        );

        let title_elem = document.getElementById("title");

        title_elem.addEventListener(
            "blur",
            async function () {
                if (title != title_elem.innerHTML) {
                    title = title_elem.innerHTML;
                    response = await api.post(
                        `profile/${id}/title/`,
                        { title },
                        localStorage.getItem("jwt")
                    );
                    if (response.error) {
                        Swal.fire(response.error);
                    }
                }
            },
            false
        );

        let name_elem = document.getElementById("name");

        name_elem.addEventListener(
            "blur",
            function () {
                if (name != name_elem.innerHTML) {
                    name = name_elem.innerHTML;
                }
            },
            false
        );
    });
</script>

<div class="row" style="margin-top:10px">
    <div class="col-12 col-sm-12 col-md-2" style="float:left;margin-right:10px">
        <img
            src="{image_url}?s=160"
            alt="{username}'s proile image"
            width="160px"
        />
    </div>
    <div class="col-12 col-sm-12 col-md-6" style="float:left">
        <table>
            <tr>
                {#if $session.user == username}
                    <td
                        ><span
                            contenteditable="true"
                            id="username"
                            style="font-size:20px;font-weight: 500;"
                            title="Click to edit">{username}</span
                        ></td
                    >
                {:else}
                    <td><span id="username">{username}</span></td>
                {/if}
            </tr>
            <tr>
                {#if $session.user == username}
                    <td
                        ><span
                            contenteditable="true"
                            id="title"
                            title="Click to edit">{title}</span
                        ></td
                    >
                {:else}
                    <td><span id="title">{title}</span></td>
                {/if}
            </tr>
            <tr>
                {#if $session.user == username}
                    <td><span contenteditable="true" id="name">{name}</span></td
                    >
                {:else}
                    <td><span id="name">{name}</span></td>
                {/if}
            </tr>
            <tr>
                {#if $session.user == username}
                    <td
                        ><span contenteditable="true" id="designation"
                            >{designation}</span
                        ></td
                    >
                {:else}
                    <td><span id="designation">{designation}</span></td>
                {/if}
            </tr>
        </table>
    </div>
    <div class="col-12 col-sm-12 col-md-3" style="display:inline">
        <table>
            <tr>
                {#if $session.user == username}
                    <td>
                        <span class="material-icons"> place </span>
                        <span contenteditable="true" id="location">
                            {location}
                        </span>
                    </td>
                {:else}
                    <td>
                        <span class="material-icons"> place </span>
                        <span id="location">
                            {location}
                        </span>
                    </td>
                {/if}
            </tr>
        </table>
    </div>
</div>
