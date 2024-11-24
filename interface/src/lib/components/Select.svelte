<script lang="ts">
    export let label: string;
    export let onChange = () => {};
</script>

<label class="label cursor-pointer" for={label}>
    <span class="mr-4">{label}</span>
</label>
<select class="select select-bordered w-full max-w-xs">
    <slot></slot>
</select>